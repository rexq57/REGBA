window.addEventListener('load', init, false);

var debugMode = false;

function init() {
  displayButtons();
  initLCD();
  resizePhone();
  initSynthesizer();
  composerSong.tempo = 125;
  loadSong(composerSong, composerAxelF);
  initControls();
  renderLCD();
}

// Axel F - Harold Faltermeyer
// http://nokia.nigelcoldwell.co.uk/tunes.html
var composerAxelF = '4g2 8.#a2 16g2 16- 16g2 8c3 8g2 8f2 4g2 8.d3 16g2 16-' +
    ' 16g2 8#d3 8d3 8#a2 8g2 8d3 8g3 16g2 16f2 16- 16f2 8d2 8a2 2.g2';

// RTTTL Nokia Tune
// https://en.wikipedia.org/wiki/Nokia_tune
var rtttlNokiaTune =
    'Nokia:o=5,d=8,b=240:E6,D6,4F#,4G#,C#6,B,4D,4E,B,A,4C#,4E,2A.';

var composerNyanCat =
    '8#f3 8#g3 16#c3 8#d3 16#c3 16d3 16#c3 8b2 8b2 8#c3 8d3 16d3 16#c3 16b2' +
    '16#c3 16#d3 16#f3 16#g3 16#d3 16#f3 16#c3 16#d3 16b2 16#c3 16b2 8#d3' +
    '8#f3 16#g3 16#d3 16#f3 16#c3 16#d3 16b2 16#c3 16#d3 16d3 16#c3 16b2' +
    '16#c3 8d3 16b2 16#c3 16#d3 16#f3 16#c3 16d3 16#c3 16b2 8#c3 8b2';
