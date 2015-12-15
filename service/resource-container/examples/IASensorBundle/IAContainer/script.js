total_calories = 0

function json_to_dom(data) {
  var walking_session = data.walking_session;
  var energy_consumption = data.energy_consumption;

  var start = walking_session.start;
  var stop = walking_session.stop;
  var orientation = walking_session.orientation;
  var destination = walking_session.destination;
  var calorie_burn = energy_consumption.calorie_burn;

  return "<div class='walking_session' style='margin-bottom: 10px'>" +
    "<div class='start'> Start: " + start + "</div>" +
    "<div class='stop'> Stop: " + stop + "</div>" +
    "<div class='orientation'> Orientation: " + orientation + "</div>" +
    "<div class='destination'> Destination: " + destination + "</div>" +
    "<div class='calorie_burn'> Calorie Burn: " + parseFloat(calorie_burn).toFixed(3) + " Cal.</div>"
}

function sum_total_calories(data) {
  return total_calories += parseFloat(data.energy_consumption.calorie_burn); 
}

function update() {
  $("#total_calorie_burn").show();
  $.getJSON("pinplace.json").done(function(data) {
    console.log(data)
    $("#current_status").html(json_to_dom(data));
    $("#total_calorie").text(sum_total_calories(data).toFixed(3)); 
  }) 
}

$(function() { 
  setInterval(update, 5000); 
})
