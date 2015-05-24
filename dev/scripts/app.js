// Get choosen Options
function saveOptions() {
    var options = {};
    //Add all textual values
    $('textarea, select, [type="hidden"], [type="password"], [type="text"], [type="number"]').each(function(){options[$(this).attr('id')] = $(this).val();});
    //Add all checkbox type values
    $('[type="radio"], [type="checkbox"]').each(function(){options[$(this).attr('id')] = $(this).is(':checked');});
    // set options to localStorage
    localStorage.setItem("customPebble", JSON.stringify(options));
    // return options
    return options;
}
//Set form values to whatever is passed in.
function setOptionsFromPebble() {
    var obj;

    if (window.location.search.substring(1) === "") obj = jQuery.parseJSON(localStorage.getItem("customPebble"));
    else obj = jQuery.parseJSON(decodeURIComponent(window.location.search.substring(1)));

    for(var key in obj) {
        if (key === "selectAlwaysTime") $("#selectAlwaysTime").val(obj["selectAlwaysTime"]).slider("refresh");
        else if (key === "inputNeededTaps") $("#inputNeededTaps").val(obj["inputNeededTaps"]);
        else if (key === "rbPebble" && obj["rbPebble"]) {
            $("input[type=radio][name=rbPebbleOrPebbleTime][value=pebble]").prop({checked: true}).checkboxradio("refresh");
            $('#colorChoosePebbleContainer').stop(true,true).show(500);
        }
        else if (key === "rbPebbleTime" && obj["rbPebbleTime"]) {
            $("input[type=radio][name=rbPebbleOrPebbleTime][value=pebbleTime]").prop({checked: true}).checkboxradio("refresh");
            $('#colorChoosePebbleTimeContainer').stop(true,true).show(500);
        }
        else if (key === "rbDefault" && obj["rbDefault"]) $("input[type=radio][name=rbFirstRowText][value=default]").prop({checked: true}).checkboxradio("refresh");
        else if (key === "rbHack" && obj["rbHack"]) $("input[type=radio][name=rbFirstRowText][value=hack]").prop({checked: true}).checkboxradio("refresh");
        else if (key === "rbBeer" && obj["rbBeer"]) $("input[type=radio][name=rbFirstRowText][value=beer]").prop({checked: true}).checkboxradio("refresh");
        else if (key === "bgBeer" && obj["rbPebbleTime"]) $("#bgBeer").val(obj["bgBeer"]).selectmenu("refresh");
        else if (key === "bgHack" && obj["rbPebbleTime"]) $("#bgHack").val(obj["bgHack"]).selectmenu("refresh");
        else if (key === "bgOclock" && obj["rbPebbleTime"]) $("#bgOclock").val(obj["bgOclock"]).selectmenu("refresh");
        else if (key === "bgTime" && obj["rbPebbleTime"]) $("#bgTime").val(obj["bgTime"]).selectmenu("refresh");
        else if (key === "bgWbg" && obj["rbPebbleTime"]) $("#bgWbg").val(obj["bgWbg"]).selectmenu("refresh");
        else if (key === "tcBeer" && obj["rbPebbleTime"]) $("#tcBeer").val(obj["tcBeer"]).selectmenu("refresh");
        else if (key === "tcHack" && obj["rbPebbleTime"]) $("#tcHack").val(obj["tcHack"]).selectmenu("refresh");
        else if (key === "tcOclock" && obj["rbPebbleTime"]) $("#tcOclock").val(obj["tcOclock"]).selectmenu("refresh");
        else if (key === "tcTime" && obj["rbPebbleTime"]) $("#tcTime").val(obj["tcTime"]).selectmenu("refresh");
        else if (key === "bgPblBeer" && obj["rbPebble"]) $("#bgPblBeer").val(obj["bgPblBeer"]).selectmenu("refresh");
        else if (key === "bgPblHack" && obj["rbPebble"]) $("#bgPblHack").val(obj["bgPblHack"]).selectmenu("refresh");
        else if (key === "bgPblOclock" && obj["rbPebble"]) $("#bgPblOclock").val(obj["bgPblOclock"]).selectmenu("refresh");
        else if (key === "bgPblTime" && obj["rbPebble"]) $("#bgPblTime").val(obj["bgPblTime"]).selectmenu("refresh");
        else if (key === "bgPblWbg" && obj["rbPebble"]) $("#bgPblWbg").val(obj["bgPblWbg"]).selectmenu("refresh");
        else if (key === "tcPblBeer" && obj["rbPebble"]) $("#tcPblBeer").val(obj["tcPblBeer"]).selectmenu("refresh");
        else if (key === "tcPblHack" && obj["rbPebble"]) $("#tcPblHack").val(obj["tcPblHack"]).selectmenu("refresh");
        else if (key === "tcPblOclock" && obj["rbPebble"]) $("#tcPblOclock").val(obj["tcPblOclock"]).selectmenu("refresh");
        else if (key === "tcPblTime" && obj["rbPebble"]) $("#tcPblTime").val(obj["tcPblTime"]).selectmenu("refresh");
        else if (key === "inputBeginWorkingTime") $("#inputBeginWorkingTime").val(obj["inputBeginWorkingTime"]);
        else if (key === "inputEndWorkingTime") $("#inputEndWorkingTime").val(obj["inputEndWorkingTime"]);
        else if (key === "inputBeerText") $("#inputBeerText").val(obj["inputBeerText"]);
        else if (key === "inputHackText") $("#inputHackText").val(obj["inputHackText"]);
        else if ($("input[type=radio][name=rbFirstRowText][value=default]").is(':checked') === !obj["rbDefault"]) $("input[type=radio][name=rbFirstRowText][value=default]").prop({checked: false}).checkboxradio("refresh");
        else console.log("key: " + key + " not defined!");
    }
}
// Document ready
$().ready(function() {
    // Show Loading Panel
    $(window).load(function() {
        $(".loader").fadeOut("slow");
    });
    // Set saved settings from Pebble
    $(function() {
        setOptionsFromPebble();
    });
    // Show Hide Color Choose Container
    $('#colorChoosePebbleContainer').hide();
    $('#colorChoosePebbleTimeContainer').hide();
    $(".radio").change(function () {
        if (this.value === "pebble") {
            $('#colorChoosePebbleTimeContainer').stop(true,true).hide(500);
            $('#colorChoosePebbleContainer').stop(true,true).show(500);
        } else if (this.value === "pebbleTime" ) {
            $('#colorChoosePebbleContainer').stop(true,true).hide(500);
            $('#colorChoosePebbleTimeContainer').stop(true,true).show(500);
        } else {
            //nothing to do here
        }
    });
    // Cancel Button
    $("#b-cancel").click(function() {
        console.log("Cancel");
        document.location = "pebblejs://close";
    });
    // Submit Button
    $("#b-submit").click(function() {
        console.log("Submit");
        var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
        console.log("Warping to: " + location);
        document.location = location;
    });
});
