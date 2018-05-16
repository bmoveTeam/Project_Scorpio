// Cuando se baja de 20px del top de la web el boton sale
window.onscroll = function() {scrollFunction()}; //Funcion de scroll
function scrollFunction() {
  if (document.body.scrollTop > 20 || document.documentElement.scrollTop > 20) {
    document.getElementById("btnup").style.display = "block";
} else {
    document.getElementById("btnup").style.display = "none";
    }
}
// Cuando se pulsa el boton ir arriba
function topFunction() {
    document.body.scrollTop = 0; // Para safari
    document.documentElement.scrollTop = 0; // Para Chrome, Firefox, IE y Opera
}
