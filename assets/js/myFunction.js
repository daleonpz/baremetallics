/* Toggle between adding and removing the "responsive" class to topnav when the user clicks on the icon */
function myFunction() {
  var x = document.getElementById("nav_bar");
  if (x.className === "nav_items") {
    x.className += " responsive";
  } else {
    x.className = "nav_items";
  }
} 
