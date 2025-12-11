import { useState, useEffect } from 'react'
import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet'
import 'leaflet/dist/leaflet.css' // <--- MUST BE HERE
import L from 'leaflet';
import './App.css'; // <--- Ensure this is imported

// --- Fix for missing Marker Icons in Vite ---
import icon from 'leaflet/dist/images/marker-icon.png';
import iconShadow from 'leaflet/dist/images/marker-shadow.png';

let DefaultIcon = L.icon({
    iconUrl: icon,
    shadowUrl: iconShadow,
    iconSize: [25, 41],
    iconAnchor: [12, 41]
});
L.Marker.prototype.options.icon = DefaultIcon;
// --------------------------------------------

function App() {
  const [fleet, setFleet] = useState({});
  const [connected, setConnected] = useState(false);

  useEffect(() => {
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    // Use window.location.hostname to work inside Docker/Localhost automatically
    const ws = new WebSocket(`${protocol}//${window.location.hostname}:8080/ws`);

    ws.onopen = () => setConnected(true);
    ws.onclose = () => setConnected(false);

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        
        // Handle case sensitivity (C++ sometimes serializes as uppercase depending on the struct)
        const id = data.id || data.ID || data.sourceID; 

        if (id) {
          // Normalize the data so the rest of the app uses lowercase 'id'
          const normalizedData = { ...data, id: id };
          
          setFleet(prev => ({
            ...prev,
            [id]: normalizedData
          }));
        } else {
            console.warn("Received JSON without an ID:", data);
        }
      } catch (e) { console.error(e); }
    };
    return () => ws.close();
  }, []);

  const ships = Object.values(fleet);

  return (
    <div className="dashboard-container">
      {/* HUD */}
      <div className="hud-overlay">
        <h3>FLEET COMMAND</h3>
        <div style={{borderBottom:'1px solid #444', paddingBottom:'5px', marginBottom:'10px'}}>
          STATUS: <span style={{color: connected ? '#0f0' : '#f00'}}>{connected ? "ONLINE" : "OFFLINE"}</span>
        </div>
        
        {ships.length === 0 && <div>Waiting for signals...</div>}

        {ships.map(ship => (
          <div key={ship.id} style={{marginBottom: '8px', borderBottom:'1px solid #333'}}>
            <strong>{ship.id}</strong>
            <div style={{display:'flex', justifyContent:'space-between'}}>
              <span>Lat: {ship.lat.toFixed(4)}</span>
              <span>Lon: {ship.lon.toFixed(4)}</span>
            </div>
          </div>
        ))}
      </div>

      {/* Map */}
      <MapContainer 
        center={[0, 0]} // Default center (Middle of ocean)
        zoom={2} 
        className="map-layer"
      >
        <TileLayer 
          attribution='&copy; OpenStreetMap'
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
        />
        
        {ships.map(ship => (
          <Marker key={ship.id} position={[ship.lat, ship.lon]}>
            <Popup>{ship.id}<br/>{ship.speed} kts</Popup>
          </Marker>
        ))}
      </MapContainer>
    </div>
  )
}

export default App