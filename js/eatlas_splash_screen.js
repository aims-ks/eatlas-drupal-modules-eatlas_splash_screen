(function ($) {
	function getURLParameters() {
		// NOTE: This can easily be done using the JavaScript URL object,
		//   but Internet Explorer doesn't support it (it's always him...)

		// window.location => Page URL
		//   .search       => URL parameters part
		//   .substring(1) => Remove the "?" at the beginning
		var parameters = {};
		var query = window.location.search.substring(1);
		if (query) {
			var key, value, pos, entries = query.split("&");
			for (var i=0; i<entries.length; i++) {
				// Separate key from value
				//   Key and value are decoded using "decodeURIComponent"
				// NOTE: It's safer to use "indexOf", in case the value contains an unencodded "="
				pos = entries[i].indexOf("=");
				if (pos < 0) {
					key = decodeURIComponent(entries[i]);
					value = "";
				} else {
					key = decodeURIComponent(entries[i].substring(0, pos));
					value = decodeURIComponent(entries[i].substring(pos+1));
				}

				// Prevent key from clashing with internal Array attributes
				// Examples: "toString", "valueOf", "isPrototypeOf", "hasOwnProperty", "get", "set", etc.
				while (typeof parameters[key] !== "undefined" && !parameters.hasOwnProperty(key)) {
					key = "_" + key;
				}

				// If first entry with this key (enter as string)
				if (typeof parameters[key] === "undefined") {
					parameters[key] = value;

				// If second entry with this key (enter as array with first value)
				} else if (typeof parameters[key] === "string") {
					var arr = [parameters[key], value];
					parameters[key] = arr;

				// If third or later entry with this key (append value to array)
				} else if (typeof parameters[key] === "array") {
					parameters[key].push(value);
				}
			}
		}
		return parameters;
	}


	$(document).ready(function() {
		// Loop through all Splash screens
		$(".block-eatlas-splash-screen").each(function() {
			var splashScreen = $(this),
				splashScreenId = splashScreen.attr('id');

			if (splashScreenId) {

				// Manage splash screen views using local storage
				if (window.localStorage) {
					var maxView = 1;
					var splashScreenViewCount = parseInt(window.localStorage.getItem(splashScreenId) || 0);

					// In case the local storage has been corrupted?
					if (isNaN(splashScreenViewCount) || splashScreenViewCount < 0) {
						splashScreenViewCount = 0;
					}

					// Clear "splashscreen" variable from the user storage if the URL parameter "reset" is present
					if (splashScreenViewCount) {
						var urlParams = getURLParameters();
						if (urlParams.hasOwnProperty('reset')) {
							// Clear the splash screen ID variable from the user storage
							window.localStorage.removeItem(splashScreenId);
							splashScreenViewCount = 0;
						}
					}

					if (splashScreenViewCount < maxView) {
						// Show the splash screen if the user have not seen it enough
						splashScreen.show();
					}
					window.localStorage.setItem(splashScreenId, splashScreenViewCount+1);

				} else {
					console.error("eAtlas Splash Screen ERROR: Local storage NOT supported!");
					// Always show the splash screen if the browser doesn't support local storage
					splashScreen.show();
				}


				// Vanish when the [X] button is clicked
				splashScreen.find(".top-close-button").click(function(splashScreen) {
					return function() {
						splashScreen.fadeOut(250);
						return false;
					};
				}(splashScreen));

				// Move up and vanish when the [V] button is clicked
				splashScreen.find(".bottom-close-button").click(function(splashScreen) {
					return function() {
						splashScreen.fadeOut(500);
						splashScreen.find(".splash-screen").animate({ top: "-100vh", bottom: "100vh" }, 500);
						return false;
					};
				}(splashScreen));

				// Move up and vanish when the overlay is clicked
				splashScreen.find(".overlay").click(function(splashScreen) {
					return function() {
						splashScreen.fadeOut(500);
						splashScreen.find(".splash-screen").animate({ top: "-100vh", bottom: "100vh" }, 500);
						return false;
					};
				}(splashScreen));
			} else {
				// This should not happen,
				// Drupal automatically assign ID to every blocks.
				console.error("eAtlas Splash Screen ERROR: The splash acreen block have NO ID.");
			}
		});
	});
}(jQuery));
