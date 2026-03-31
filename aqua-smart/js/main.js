document.addEventListener("DOMContentLoaded", () => {
    // Intersection Observer for Scroll Animations
    const observerOptions = {
        root: null,
        rootMargin: '0px',
        threshold: 0.15 // Triggers when 15% of the element is visible
    };

    const observer = new IntersectionObserver((entries, observer) => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('active');
                // Optional: Stop observing once revealed if you don't want it to hide again on scroll up
                // observer.unobserve(entry.target); 
            }
        });
    }, observerOptions);

    // Select all elements with the 'reveal' class
    const revealElements = document.querySelectorAll('.reveal');
    
    revealElements.forEach(el => {
        observer.observe(el);
    });
});