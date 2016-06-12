/*
Libray:   Michael Gries (c)2009-2015
Creation: 2009-APR-06 (VRC Monitor)
Modified: 2016-JUN-12 (WDG Network Monitoring)
Version:  16.6.11
Notes:    uses SSI functionality; 
          therefore file extention requires: *.shtm
*/

function drawLimit(limit, color, boolTrue) {
var x;
var y = D.ScreenY(limit); 
    if (boolTrue) {
        for (x = left - 5; x < right; x++) {
            new Pixel(x, y, color);
        } // horizontal line
    }
}

function drawGraph() {
    var xPixelSize = 1440; // must be xSize in script VRCday.shtm
    var yStatusValueLine = 100.0 // see s1[1][xxxx] parameter
   
       
    // Parameter N[1]
    var nSourceFull;
    nSourceFull = N[1][1];  // vrc_YYYY-MM-DD.log (0-18 characters)
        nSource = nSourceFull.substring(4,18);

    // Parameter S[1]
    sBetriebsart = B[1][1];
        if (sBetriebsart == "00") {
            sBetriebsart = "Sommerbetrieb"
        }
        if (sBetriebsart == "01") {
            sBetriebsart = "Winterbetrieb"
        }

    // Parameter C[1]
    sBDauer = C[1][1]; // totals of heating period in seconds

    // Parameter C[2]
    sBStart = C[2][1]; // number of heating periods 

    // Parameter T[1]
    var xPixel = 0;
    for (var element in T[1]) {
        if (++xPixel <= xPixelSize) {
            tAussen = T[1][element];
            new Pixel(D.ScreenX(xPixel/xFactor), D.ScreenY(tAussen), cAussen);
        }
    }
        
    // Parameter T[2]
    var xPixel = 0;
    for (var element in T[2]) {
        if (++xPixel <= xPixelSize) {
            tWasser = T[2][element];
            new Pixel(D.ScreenX(xPixel/xFactor), D.ScreenY(tWasser), cWasser);
        }
    }

    // Parameter T[3]
    var xPixel = 0;
    for (var element in T[3]) {
        if (++xPixel <= xPixelSize) {
            tKessel = T[3][element];
            new Pixel(D.ScreenX(xPixel/xFactor), D.ScreenY(tKessel), cKessel);
        }
    }

    // Parameter T[4]
    var xPixel = 0;
    for (var element in T[4]) {
        if (++xPixel <= xPixelSize) {
            tHKurve = T[4][element];
            new Pixel(D.ScreenX(xPixel/xFactor), D.ScreenY(tHKurve), cHKurve);
        }
    }
    
    // Parameter S[1]
    // attention (source: http://www.w3schools.com/js/js_numbers.asp): 
    // 'Never write a number with a leading zero (like 07)'
    // 'Some JavaScript versions interpret numbers as octal if they are written with a leading zero'
    // In this application leading zero is used by sending device (python script)
    // i.e: S[1][1000]="0256" == 174
    var xPixel = 0;
    bService = false;  // last data record indicates Service Bar to be displayed
    for (var element in S[1]) {
        if (++xPixel <= xPixelSize) {
            sBetrieb = S[1][element];
            sBetriebsstatus = "t.b.d."; // default
            switch (sBetrieb) {
            case 0:    
                sBetriebsstatus = "Bereitschaft (kein Heizbetrieb)";
                bService = false; // no service (reset indication)
                break; 
            case 1:
                sBetriebsstatus = "Heizbetrieb";
                newPixelBlock(xPixel, yStatusValueLine, cKessel);
                bService = false; // no service (reset indication)
                break;
            case 174:  
            case 256:  // due to leading 0 equals octet 174 (compare IE developer tool F12)
                sBetriebsstatus = "Wartung";
                new Pixel(D.ScreenX(xPixel/xFactor), D.ScreenY(yStatusValueLine+1), cService);
                bService = true;
                break;
            case 175:
            case 257:  // equal octet 175 (compare IE developer tool F12)
                sBetriebsstatus = "Heizbetrieb &amp; Wartung";
                newPixelBlock(xPixel, yStatusValueLine, cKessel);
                bService = true;
                break;
            case 1024:
                sBetriebsstatus = "Warmwasser (Anforderung)";
                newPixelBlock(xPixel, yStatusValueLine, cKessel);
                bService = false; // no service (reset indication)
                break;
            case 1026:
                sBetriebsstatus = "Warmwasser";
                newPixelBlock(xPixel, yStatusValueLine, cWasser);
                bService = false; // no service (reset indication)
                break;
            case 1280:  // Speicherladung (Warmwasser) + Wartung
                sBetriebsstatus = "Speicherladung &amp; Wartung";
                new Pixel(D.ScreenX(xPixel/xFactor), D.ScreenY(yStatusValueLine+1), cService);
                // in between no Pixel (i.e. frame line)
                new Pixel(D.ScreenX(xPixel/xFactor), D.ScreenY(yStatusValueLine-1), cWasser);
                bService = true;
                break;
            case 1282:  // Warmwasser + Pumpe + Wartung
                sBetriebsstatus = "Warmwasser";
                newPixelBlock(xPixel, yStatusValueLine, cWasser);
                bService = true;
                break;
            case 1538:
                sBetriebsstatus = "Warmwasser + ???";
                newPixelBlock(xPixel, yStatusValueLine, cWasser);
                bService = false; // no service (reset indication)
                break;
            }
        }
    }
    // data record evaluation and displaying ends here
    // evaluate Service request
    if (bService) // service request detected
    {
        newBarService();  // dynamically display service bar
    }
}

function  newPixelBlock(x, y, color) {
    new Pixel(D.ScreenX(x/xFactor), D.ScreenY(y-1), color);
    new Pixel(D.ScreenX(x/xFactor), D.ScreenY(y), color);
    new Pixel(D.ScreenX(x/xFactor), D.ScreenY(y+1), color);
}

function newBarService(){
    // Bar coordinates hard-coded
    // Bar: Service (Wartung)
    var bar1Text = "Service";
    var bar1Titel = "Systemmeldung Wartung";
    new Bar(60+1440-150, 120+240+30, 60+1440, 120+240+30+20, cService, bar1Text, cRED, bar1Titel);
}

function fuehrendeNull(wert)
{
  if (wert<10) return "0" + parseInt(wert);
  else return parseInt(wert);
}

function SekundenumwandelnHMS(Sekundenzahl)   // Stunden, Minuten, Sekunden
{
 Sekundenzahl = Math.abs(Sekundenzahl)
 return fuehrendeNull((Sekundenzahl/60/60)%24) + ":" +
        fuehrendeNull((Sekundenzahl/60)%60) + ":" + fuehrendeNull(Sekundenzahl%60);
}
function Sekundenumwandeln(Sekundenzahl)   // Tage, Stunden, Minuten, Sekunden
{
 Sekundenzahl = Math.abs(Sekundenzahl)
 return parseInt(Sekundenzahl/60/60/24)+ " Tage " + fuehrendeNull((Sekundenzahl/60/60)%24) + ":" +
        fuehrendeNull((Sekundenzahl/60)%60) + ":" + fuehrendeNull(Sekundenzahl%60);
}
         
function nextDiagram() {
    window.location.href = "http://www.gries.name/VRC/VRCday.shtm";
}