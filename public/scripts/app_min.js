function saveOptions(){var e={};return $('textarea, select, [type="hidden"], [type="password"], [type="text"], [type="number"]').each(function(){e[$(this).attr("id")]=$(this).val()}),$('[type="radio"], [type="checkbox"]').each(function(){e[$(this).attr("id")]=$(this).is(":checked")}),e}$().ready(function(){$("#colorChoosePebbleContainer").hide(),$("#colorChoosePebbleTimeContainer").hide(),$(".radio").change(function(){"pebble"==this.value?($("#colorChoosePebbleTimeContainer").stop(!0,!0).hide(500),$("#colorChoosePebbleContainer").stop(!0,!0).show(500)):($("#colorChoosePebbleContainer").stop(!0,!0).hide(500),$("#colorChoosePebbleTimeContainer").stop(!0,!0).show(500))}),$("#b-cancel").click(function(){console.log("Cancel"),document.location="pebblejs://close"}),$("#b-submit").click(function(){console.log("Submit");var e="pebblejs://close#"+encodeURIComponent(JSON.stringify(saveOptions()));console.log("Warping to: "+e),console.log(e),document.location=e});var e=jQuery.parseJSON(decodeURIComponent(window.location.search.substring(1)));for(key in e)$("#"+[key]).val(e[key]),$("#"+[key]).val(e[key]).slider("refresh")});