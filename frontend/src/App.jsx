import { useState, useEffect } from 'react'
import { MapContainer, TileLayer, Marker, Popup, useMap } from 'react-leaflet'
import 'leaflet/dist/leaflet.css' // <--- CRITICAL: Map looks broken without this
import './App.css'

import L from 'leaflet';

// Fix broken icons in Vite
import icon from 'leaflet/dist/images/marker-icon.png';
import iconShadow from 'leaflet/dist/images/marker-shadow.png';

let DefaultIcon = L.icon({
    iconUrl: icon,
    shadowUrl: iconShadow,
    iconSize: [25, 41],
    iconAnchor: [12, 41]
});

L.Marker.prototype.options.icon = DefaultIcon;

// Helper to auto-pan the map when GPS updates
function RecenterMap({ lat, lon }) {
  const map = useMap();
  useEffect(() => {
    map.setView([lat, lon]);
  }, [lat, lon]);
  return null;
}

function App() {
  // 1. Define State
  const [gpsData, setGpsData] = useState({
    lat: 48.1351, // Default (Munich)
    lon: 11.5820,
    speed: 0.0,
    course: 0.0,
    sats: 0,
    isValid: false
  });

  const [connected, setConnected] = useState(false);

  // 2. WebSocket Hook
  useEffect(() => {
    const ws = new WebSocket("ws://localhost:8080/ws");

    ws.onopen = () => {
      console.log("Connected to C++ Backend");
      setConnected(true);
    };

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        // Only update if data is valid coordinates
        if (data.lat && data.lon) {
          setGpsData(data);
        }
      } catch (e) {
        console.error("JSON Parse Error", e);
      }
    };

    ws.onclose = () => setConnected(false);

    // Cleanup on unmount
    return () => ws.close();
  }, []);

  return (
    <div className="dashboard-container">
      
      {/* 3. The Heads Up Display (HUD) */}
      <div className="hud-overlay">
        <h3>NMEA ENGINE</h3>
        <div className="hud-row">
          <span className="hud-label">STATUS:</span>
          <span className="hud-value" style={{color: connected ? '#0f0' : '#f00'}}>
            {connected ? "ONLINE" : "DISCONNECTED"}
          </span>
        </div>
        <div className="hud-row">
          <span className="hud-label">LAT:</span>
          <span className="hud-value">{gpsData.lat.toFixed(6)}</span>
        </div>
        <div className="hud-row">
          <span className="hud-label">LON:</span>
          <span className="hud-value">{gpsData.lon.toFixed(6)}</span>
        </div>
        <div className="hud-row">
          <span className="hud-label">SPEED:</span>
          <span className="hud-value">{gpsData.speed.toFixed(1)} kts</span>
        </div>
        <div className="hud-row">
          <span className="hud-label">COURSE:</span>
          <span className="hud-value">{gpsData.course.toFixed(1)}°</span>
        </div>
      </div>

      {/* 4. The Map */}
      <MapContainer 
        center={[gpsData.lat, gpsData.lon]} 
        zoom={13} 
        scrollWheelZoom={true} 
        className="map-layer"
      >
        <TileLayer
          attribution='&copy; OpenStreetMap contributors'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        <Marker position={[gpsData.lat, gpsData.lon]}>
          <Popup>
            Current Position<br />
            Speed: {gpsData.speed} kts
          </Popup>
        </Marker>
        
        {/* Auto-follow the boat */}
        <RecenterMap lat={gpsData.lat} lon={gpsData.lon} />
      </MapContainer>
    </div>
  )
}

export default App