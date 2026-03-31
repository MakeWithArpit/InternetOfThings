document.addEventListener("DOMContentLoaded", () => {
    const canvas = document.getElementById('tankCanvas');
    if (!canvas) return;

    // 1. Scene setup
    const scene = new THREE.Scene();

    // 2. Camera setup
    const camera = new THREE.PerspectiveCamera(45, canvas.clientWidth / canvas.clientHeight, 0.1, 1000);
    camera.position.set(0, 3, 10); // Positioned slightly above and back

    // 3. Renderer setup
    const renderer = new THREE.WebGLRenderer({ canvas: canvas, alpha: true, antialias: true });
    renderer.setSize(canvas.clientWidth, canvas.clientHeight);
    renderer.setPixelRatio(window.devicePixelRatio);

    // 4. Create the Outer Glass Tank
    const tankGeometry = new THREE.CylinderGeometry(2, 2, 5, 32);
    const tankMaterial = new THREE.MeshPhysicalMaterial({
        color: 0xffffff,
        transparent: true,
        opacity: 0.15,
        roughness: 0.1,
        metalness: 0.1,
        side: THREE.DoubleSide
    });
    const tank = new THREE.Mesh(tankGeometry, tankMaterial);
    scene.add(tank);

    // Add glowing edges to the tank
    const edgesGeometry = new THREE.EdgesGeometry(tankGeometry);
    const edgesMaterial = new THREE.LineBasicMaterial({ color: 0x00f0ff, transparent: true, opacity: 0.3 });
    const tankEdges = new THREE.LineSegments(edgesGeometry, edgesMaterial);
    tank.add(tankEdges);

    // 5. Create the Water inside
    const waterGeometry = new THREE.CylinderGeometry(1.9, 1.9, 5, 32);
    // Move geometry origin to bottom so it scales upwards
    waterGeometry.translate(0, 2.5, 0); 
    
    const waterMaterial = new THREE.MeshStandardMaterial({
        color: 0x00f0ff,
        emissive: 0x0055aa,
        emissiveIntensity: 0.4,
        transparent: true,
        opacity: 0.85,
        roughness: 0.2,
        metalness: 0.8
    });
    const water = new THREE.Mesh(waterGeometry, waterMaterial);
    
    // Position water at the bottom of the tank
    water.position.y = -2.5;
    scene.add(water);

    // 6. Lighting
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
    scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 1);
    directionalLight.position.set(5, 5, 5);
    scene.add(directionalLight);

    // Add a cyan point light to enhance the neon glow
    const pointLight = new THREE.PointLight(0x00f0ff, 2, 10);
    pointLight.position.set(0, -2, 3);
    scene.add(pointLight);

    // Group for rotation
    const tankGroup = new THREE.Group();
    tankGroup.add(tank);
    tankGroup.add(water);
    scene.add(tankGroup);

    // 7. Animation Loop
    let time = 0;
    function animate() {
        requestAnimationFrame(animate);

        // Slowly rotate the entire tank
        tankGroup.rotation.y += 0.005;
        tankGroup.rotation.z = Math.sin(time * 0.5) * 0.05; // Gentle sway

        // Simulate water filling and draining (Sine wave animation)
        time += 0.015;
        // Scale between 0.2 (20%) and 0.9 (90%)
        const waterLevel = (Math.sin(time) + 1) / 2; // normalizes sin from 0 to 1
        const currentScale = 0.2 + (waterLevel * 0.7); 
        
        water.scale.y = currentScale;

        renderer.render(scene, camera);
    }

    animate();

    // 8. Handle Window Resize
    window.addEventListener('resize', () => {
        const width = canvas.clientWidth;
        const height = canvas.clientHeight;
        renderer.setSize(width, height, false);
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
    });
});