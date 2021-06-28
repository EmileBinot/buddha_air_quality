static float tempC = 0.0;
static float humidity = 0.0;
static int oneDayCO2[ONE_DAY];
static int tenMinutesCO2[TEN_MINUTES];
static int oneDayTVOC[ONE_DAY];
static int tenMinutesTVOC[TEN_MINUTES];
static int tenMinuteCounter = 0; // counter for the minute buffer
static int oneDayCounter = 0; // counter for the daily buffer

// WiFi setup
static const char* ssid = "Livebox-CA3E";
static const char* password = "NoProblemo84";
static WiFiServer server(80);

// HTML code
static const char headerHTML[] PROGMEM =
   "<!DOCTYPE html>"
   "<html>"
   "   <head>"
   "      <title>CO2 &amp; TVOC Monitor</title>"
   "      <meta http-equiv=\"refresh\" content=\"10\">"
   "      <link rel=\"stylesheet\" href=\"//cdn.jsdelivr.net/chartist.js/latest/chartist.min.css\" type=\"text/css\">"
   "      <script src=\"//cdn.jsdelivr.net/chartist.js/latest/chartist.min.js\" type=\"application/javascript\"></script>"
   "      <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\" type=\"text/css\">"
   "   </head>"
   "   <body>"
   "      <div class=\"container\">"
   "         <h1>CO2 Monitor</h1>"
   "         <div class=\"row\">"
   "            <div class=\"col\">"
   "               <h2>10 Minute Stats</h2>"
   "               <div id=\"chart-minutes\" class=\"ct-chart ct-perfect-fourth\"></div>"
   "               <script type=\"application/javascript\">"
   "                  var data = { labels: [], series: [";

static const char middle1HTML[] PROGMEM =
   "                  ] };"
   "                  var options = { showPoint: false, lineSmooth: false, axisX: { showGrid: false, showLabel: true }, axisY: { offset: 60, labelInterpolationFnc: function(value) { return value.toString(); } } };"
   "                  new Chartist.Line('#chart-minutes', data, options);"
   "               </script>"
   "            </div>"
   "            <div class=\"col\">"
   "               <h2>24 Hour Stats</h2>"
   "               <div id=\"chart-day\" class=\"ct-chart ct-perfect-fourth\"></div>"
   "               <script type=\"application/javascript\">"
   "                  var data = { labels: [], series: [";

static const char middle2HTML[] PROGMEM =
   "                  ] };"
   "                  var options = { showPoint: false, lineSmooth: false, axisX: { showGrid: false, showLabel: true }, axisY: { offset: 60, labelInterpolationFnc: function(value) { return value.toString(); } } };"
   "                  new Chartist.Line('#chart-day', data, options);"
   "               </script>"
   "            </div>"
   "         </div>"
   "         <div class=\"row\">"
   "            <div class=\"col\">";


static const char footerHTML[] PROGMEM =
   "            </div>"
   "         </div>"
   "      </div>"
   "      <script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\" type=\"application/javascript\"></script>"
   "      <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\" type=\"application/javascript\"></script>"
   "      <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\" type=\"application/javascript\"></script>"
   "   </body>"
   "</html>";