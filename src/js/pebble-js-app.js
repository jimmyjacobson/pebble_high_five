var username = localStorage.getItem("username");

/**************** Pebble helpers ****************/

var hasKey = function(dict, key) {
  return typeof dict.payload[key] !== "undefined";
};

var getValue = function(dict, key) {
  if(hasKey(dict, key)) {
    return "" + dict.payload[key];
  } else {
    console.log("ERROR: Key '" + key + "' does not exist in received dictionary");
    return undefined;
  }
};

/************** Configuration *******************/

Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + JSON.stringify(e.payload));
    var count = getValue(e, 1);
    var req = new XMLHttpRequest();
    
    if (!username || (typeof username == 'undefined')) {
      username = "Anonymous_" + Math.floor(Math.random()*100000000001);
    }

    req.open('PUT', 'https://pebble-high-five.firebaseio.com/users/' + username + '.json');
    req.setRequestHeader("Content-Type", "application/json;charset=UTF-8");

    var data = {count: count};
    req.send(JSON.stringify(data));
  }
);

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('https://pebble-high-five.firebaseapp.com/form.html?app=true');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  var options = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(options));

  username = options.name;
  localStorage.setItem("username", username);
});