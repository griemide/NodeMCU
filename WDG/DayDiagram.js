/*
Libray:   Michael Gries (c)2009-2015
Creation: 2009-APR-06  (VRC Monitor)
Modified: 2016-JUN-12 (WDG Network Monitoring)
Version:  16.6.11
*/

var now = new Date();
var dayOfMonth = now.getDate();
    dayOfMonth = ((dayOfMonth<10) ? "0"+dayOfMonth : dayOfMonth);
var month = 1 + now.getMonth();
    month = ((month<10) ? "0" + month : month);
var monthAndDay = new String();
    monthAndDay = month + "/" + dayOfMonth; // "mm/dd"
var year = now.getFullYear();     
var level = new String();
var nSource = new String();
var tAussen = new String(); var cAussen = "#00FF00"; //green
var tWasser = new String(); var cWasser = "#0000FF"; //blue
var tKessel = new String(); var cKessel = "#FF0000"; //red
var tHKurve = new String(); var cHKurve = "#663300"; //brown
var sBDauer = new String(); var cBDauer = "#808080"; //dark grey
var sBStart = new String(); var cBStart = "#808080"; //dark grey
                             var cLimit = "#808080"; //dark grey
                           var cService = "#FFCC00"; //orange

var sBetrieb = new String();
var sBetriebsstatus = new String();
var sBetriebsart = new String();
var cBLACK = "#000000"; var cWHITE = "#FFFFFF"; var cGREY = "#808080"; var cRED = "#FF0000";

var D = new Diagram();
var left = 60; var top = 120;
var xLow = 0; var xHigh = 24; var xFactor = 60;
var sizeX = (xHigh-xLow) * xFactor; // number of minutes per day
var yLow = -20; var yHigh = 100; var yFactor = 2;
var sizeY = (yHigh-yLow) * yFactor; // = no of Pixel
var right = left + sizeX;
var bottom = top + sizeY; // Pixel=4000mv/20=200
D.SetFrame(60, 120, 60+1440, 120+240);
D.SetBorder(0, 24, -20, 100);
D.XGridDelta = 1; D.YGridDelta = 10;
D.XScale = 1; D.YScale = 1; // 1=scale numeric
D.SetText("<b>[h]</b>", "<b>[°C]</b>", "<B>Betriebsdaten Heizung - Temperaturen[°C] / Heizkurve / Betriebstatus</B>");
var boolShowDim = true;
var drawTooltip = "show next diagram"; var drawAction = "nextDiagram()";
D.Draw("#DEDEDE", "#000000", boolShowDim, drawTooltip, drawAction); // bgd-color: lightgrey

function buildDayDiagram()   // build diagram contents
{
    document.open();
    var legTop = bottom + 30;
    var legSize = 20;
    // set legend (Bars) first
    // Bar: Referenztemperatur
    var bar0Text = "Referenz: 0 °C";
    var bar0Titel = "0 Grad Marke";
    new Bar(60+901, 120+240+30, 60+1440, 120+240+30+20, cGREY, bar0Text, cBLACK, bar0Titel);
    // now data records should be evaluated and displayed
    // drawLimit() should be used before drawGraph() to allow that 0 Degree values will not be overwritten
    drawLimit(0, cLimit, true); // value in °C; draw Limit first (to allow pixel visible on line)
    drawGraph();
    // next Bars needs to be called after evaluation of data records in function drawGraph()
    // Bar: Aussentemperatur
    var bar1Text = "Aussen: " + tAussen + " °C";
    var bar1Titel = "Aussentemperaturfühler";
    new Bar(60, 120+240+30, 60+150, 120+240+30+20, cAussen, bar1Text, cBLACK, bar1Titel);
    // Bar: Wassertemperatur
    var bar1Text = "Wasser: " + tWasser + " °C";
    var bar1Titel = "Temperatur Warmwasserspeicher";
    new Bar(60+151, 120+240+30, 60+300, 120+240+30+20, cWasser, bar1Text, cWHITE, bar1Titel);
    // Bar: Kesseltemperatur
    var bar1Text = "Kessel: " + tKessel + " °C";
    var bar1Titel = "Temperatur Brennerkessel";
    new Bar(60+301, 120+240+30, 60+450, 120+240+30+20, cKessel, bar1Text, cWHITE, bar1Titel);
    // Bar: Heizkurventemp.
    var bar1Text = "H-Kurve: " + tHKurve + " °C";
    var bar1Titel = "Witterungsgef&uuml;hrte Vorlauftemperatur";
    new Bar(60+451, 120+240+30, 60+600, 120+240+30+20, cHKurve, bar1Text, cWHITE, bar1Titel);
    // Bar: Brennerdauer
    var iBDauerStunden = SekundenumwandelnHMS(parseInt(sBDauer));
    var bar1Text = "Dauer: " + iBDauerStunden;
    var bar1Titel = "Brennerdauer " + sBDauer + " s";
    new Bar(60+601, 120+240+30, 60+750, 120+240+30+20, cBDauer, bar1Text, cWHITE, bar1Titel);
    // Bar: Brennerstarts
    var bar1Text = "Starts: " + sBStart;
    var bar1Titel = sBStart + " Brennerstarts erfasst";
    new Bar(60+751, 120+240+30, 60+900, 120+240+30+20, cBStart, bar1Text, cWHITE, bar1Titel);
    //
    // new line of legend
    //
    // Bar: Betriebszustand (letzter Datensatz)
    var barTitel = "letzter dargestellter Betriebszustand";
    new Bar(60+000, 120+240+30+21, 60+300, 120+240+30+21+20, cBLACK, "Status: " + sBetriebsstatus, cWHITE, barTitel);
    // Bar: Dateiname (Datenquelle))
    var barTitel = "Rohdatendatei";
    new Bar(60+301, 120+240+30+21, 60+450, 120+240+30+21+20, cBLACK, nSource, cWHITE, barTitel);
    // Bar: Betriebsart
    var barTitel = "Betriebsart (Sommer / Winter)";
    new Bar(60+451, 120+240+30+21, 60+600, 120+240+30+21+20, cBLACK, sBetriebsart, cWHITE, barTitel);
    // Bar: Tagesverbrauch
    var fDurchsatz = 1.91; // kg/h   Düse 60°ST Danfoss
    var fDichteOel = 0.83; // kg/l   Heizoel EL nach DIN 51603
    var fLiterProH = fDurchsatz / fDichteOel; // Durchsatz in l/h
    var fBDauer = parseFloat(sBDauer);
    var fVerbrauch = fBDauer / 3600 * fLiterProH;
    var sVerbrauch = "Verbrauch: " + fVerbrauch.toFixed(1) + " l";
    var barTitel = "Tagesverbrauch bei Düsendurchsatz: " + fLiterProH.toFixed(2) + " (l/h)";
    new Bar(60+601, 120+240+30+21, 60+750, 120+240+30+21+20, cBLACK, sVerbrauch, cWHITE, barTitel);
    // Bar: Heizwert
    var fLeistungProLiter = 9.8; // KW/l   Heizwert Heizoel EL
    var fEnergie = fVerbrauch * fLeistungProLiter;
    var sEnergie = "Energie: " + fEnergie.toFixed(0) + " KWh";
    var barTitel = "errechnete Energieverbrauch nach Brennerstunden bzw. Litern";
    new Bar(60+751, 120+240+30+21, 60+900, 120+240+30+21+20, cBLACK, sEnergie, cWHITE, barTitel);
    // Bar: aktuelle Uhrzeit des Clients
    var barTitel = "aktuelle Uhrzeit";
    new Bar(60+901, 120+240+30+21, 60+1440,120+240+30+21+20, cBLACK, now, cWHITE, barTitel);
    document.close();
}