function saveOptions(){var e={};return $('textarea, select, [type="hidden"], [type="password"], [type="text"], [type="number"]').each(function(){e[$(this).attr("id")]=$(this).val()}),$('[type="radio"], [type="checkbox"]').each(function(){e[$(this).attr("id")]=$(this).is(":checked")}),localStorage.setItem("customPebble",JSON.stringify(e)),e}function setOptionsFromPebble(){var e;e=jQuery.parseJSON(""===window.location.search.substring(1)?localStorage.getItem("customPebble"):decodeURIComponent(window.location.search.substring(1)));for(var b in e)"selectAlwaysTime"===b?$("#selectAlwaysTime").val(e.selectAlwaysTime).slider("refresh"):"inputNeededTaps"===b?$("#inputNeededTaps").val(e.inputNeededTaps):"rbPebble"===b&&e.rbPebble?($("input[type=radio][name=rbPebbleOrPebbleTime][value=pebble]").prop({checked:!0}).checkboxradio("refresh"),$("#colorChoosePebbleContainer").stop(!0,!0).show(500)):"rbPebbleTime"===b&&e.rbPebbleTime?($("input[type=radio][name=rbPebbleOrPebbleTime][value=pebbleTime]").prop({checked:!0}).checkboxradio("refresh"),$("#colorChoosePebbleTimeContainer").stop(!0,!0).show(500)):"rbDefault"===b&&e.rbDefault?$("input[type=radio][name=rbFirstRowText][value=default]").prop({checked:!0}).checkboxradio("refresh"):"rbHack"===b&&e.rbHack?$("input[type=radio][name=rbFirstRowText][value=hack]").prop({checked:!0}).checkboxradio("refresh"):"rbBeer"===b&&e.rbBeer?$("input[type=radio][name=rbFirstRowText][value=beer]").prop({checked:!0}).checkboxradio("refresh"):"bgBeer"===b&&e.rbPebbleTime?$("#bgBeer").val(e.bgBeer).selectmenu("refresh"):"bgHack"===b&&e.rbPebbleTime?$("#bgHack").val(e.bgHack).selectmenu("refresh"):"bgOclock"===b&&e.rbPebbleTime?$("#bgOclock").val(e.bgOclock).selectmenu("refresh"):"bgTime"===b&&e.rbPebbleTime?$("#bgTime").val(e.bgTime).selectmenu("refresh"):"bgWbg"===b&&e.rbPebbleTime?$("#bgWbg").val(e.bgWbg).selectmenu("refresh"):"tcBeer"===b&&e.rbPebbleTime?$("#tcBeer").val(e.tcBeer).selectmenu("refresh"):"tcHack"===b&&e.rbPebbleTime?$("#tcHack").val(e.tcHack).selectmenu("refresh"):"tcOclock"===b&&e.rbPebbleTime?$("#tcOclock").val(e.tcOclock).selectmenu("refresh"):"tcTime"===b&&e.rbPebbleTime?$("#tcTime").val(e.tcTime).selectmenu("refresh"):"bgPblBeer"===b&&e.rbPebble?$("#bgPblBeer").val(e.bgPblBeer).selectmenu("refresh"):"bgPblHack"===b&&e.rbPebble?$("#bgPblHack").val(e.bgPblHack).selectmenu("refresh"):"bgPblOclock"===b&&e.rbPebble?$("#bgPblOclock").val(e.bgPblOclock).selectmenu("refresh"):"bgPblTime"===b&&e.rbPebble?$("#bgPblTime").val(e.bgPblTime).selectmenu("refresh"):"bgPblWbg"===b&&e.rbPebble?$("#bgPblWbg").val(e.bgPblWbg).selectmenu("refresh"):"tcPblBeer"===b&&e.rbPebble?$("#tcPblBeer").val(e.tcPblBeer).selectmenu("refresh"):"tcPblHack"===b&&e.rbPebble?$("#tcPblHack").val(e.tcPblHack).selectmenu("refresh"):"tcPblOclock"===b&&e.rbPebble?$("#tcPblOclock").val(e.tcPblOclock).selectmenu("refresh"):"tcPblTime"===b&&e.rbPebble?$("#tcPblTime").val(e.tcPblTime).selectmenu("refresh"):$("input[type=radio][name=rbFirstRowText][value=default]").is(":checked")===!e.rbDefault?$("input[type=radio][name=rbFirstRowText][value=default]").prop({checked:!1}).checkboxradio("refresh"):console.log("key: "+b+" not defined!")}$().ready(function(){$(window).load(function(){$(".loader").fadeOut("slow")}),$(function(){setOptionsFromPebble()}),$("#colorChoosePebbleContainer").hide(),$("#colorChoosePebbleTimeContainer").hide(),$(".radio").change(function(){"pebble"===this.value?($("#colorChoosePebbleTimeContainer").stop(!0,!0).hide(500),$("#colorChoosePebbleContainer").stop(!0,!0).show(500)):"pebbleTime"===this.value&&($("#colorChoosePebbleContainer").stop(!0,!0).hide(500),$("#colorChoosePebbleTimeContainer").stop(!0,!0).show(500))}),$("#b-cancel").click(function(){console.log("Cancel"),document.location="pebblejs://close"}),$("#b-submit").click(function(){console.log("Submit");var e="pebblejs://close#"+encodeURIComponent(JSON.stringify(saveOptions()));console.log("Warping to: "+e),document.location=e})});