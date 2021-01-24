var composerNotesBitmap = {};
composerNotesBitmap.width = 16;
composerNotesBitmap.height = 6;

// It'd be cool if we could initialize a Uint8Array like this.
//composerNotesBitmap.buffer = new ArrayBuffer(
//  composerNotesBitmap.width * composerNotesBitmap.height
//);
//composerNotesBitmap.view = new Uint8Array(composerNotesBitmap.buffer);
composerNotesBitmap.bitmap = [
  0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1,
  0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,
  0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0,
  1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0,
  0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0
];

var deadLCD = {};
deadLCD.bitmap = [];
function deadLCDInit(width, height) {
  deadLCD.width = width;
  deadLCD.height = height;
  var size = width * height;
  for (var i = 0; i < size; i++) {
    deadLCD.bitmap[i] = 1;
  }
}

var notesRemaining_0 = {};
notesRemaining_0.width = 5;
notesRemaining_0.height = 7;
notesRemaining_0.bitmap = [
  0, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0
];

var notesRemaining_1 = {};
notesRemaining_1.width = 5;
notesRemaining_1.height = 7;
notesRemaining_1.bitmap = [
  0, 0, 1, 0, 0,
  0, 1, 1, 0, 0,
  0, 0, 1, 0, 0,
  0, 0, 1, 0, 0,
  0, 0, 1, 0, 0,
  0, 0, 1, 0, 0,
  0, 0, 1, 0, 0
];

var notesRemaining_2 = {};
notesRemaining_2.width = 5;
notesRemaining_2.height = 7;
notesRemaining_2.bitmap = [
  1, 1, 1, 0, 0,
  0, 0, 0, 1, 0,
  0, 0, 0, 1, 0,
  0, 1, 1, 0, 0,
  1, 0, 0, 0, 0,
  1, 0, 0, 0, 0,
  1, 1, 1, 1, 0
];

var notesRemaining_3 = {};
notesRemaining_3.width = 5;
notesRemaining_3.height = 7;
notesRemaining_3.bitmap = [
  1, 1, 1, 0, 0,
  0, 0, 0, 1, 0,
  0, 0, 0, 1, 0,
  0, 1, 1, 0, 0,
  0, 0, 0, 1, 0,
  0, 0, 0, 1, 0,
  1, 1, 1, 0, 0
];

var notesRemaining_4 = {};
notesRemaining_4.width = 5;
notesRemaining_4.height = 7;
notesRemaining_4.bitmap = [
  0, 0, 0, 1, 0,
  0, 0, 1, 1, 0,
  0, 1, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 1, 1, 1, 0,
  0, 0, 0, 1, 0,
  0, 0, 0, 1, 0
];

var notesRemaining_5 = {};
notesRemaining_5.width = 5;
notesRemaining_5.height = 7;
notesRemaining_5.bitmap = [
  1, 1, 1, 0, 0,
  1, 0, 0, 0, 0,
  1, 1, 1, 0, 0,
  0, 0, 0, 1, 0,
  0, 0, 0, 1, 0,
  0, 0, 0, 1, 0,
  1, 1, 1, 0, 0
];

var notesRemaining_6 = {};
notesRemaining_6.width = 5;
notesRemaining_6.height = 7;
notesRemaining_6.bitmap = [
  0, 1, 1, 0, 0,
  1, 0, 0, 0, 0,
  1, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0
];

var notesRemaining_7 = {};
notesRemaining_7.width = 5;
notesRemaining_7.height = 7;
notesRemaining_7.bitmap = [
  1, 1, 1, 1, 0,
  0, 0, 0, 1, 0,
  0, 0, 1, 0, 0,
  0, 0, 1, 0, 0,
  0, 1, 0, 0, 0,
  0, 1, 0, 0, 0,
  0, 1, 0, 0, 0
];

var notesRemaining_8 = {};
notesRemaining_8.width = 5;
notesRemaining_8.height = 7;
notesRemaining_8.bitmap = [
  0, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  0, 1, 1, 0, 0
];

var notesRemaining_9 = {};
notesRemaining_9.width = 5;
notesRemaining_9.height = 7;
notesRemaining_9.bitmap = [
  0, 1, 1, 0, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  1, 0, 0, 1, 0,
  0, 1, 1, 1, 0,
  0, 0, 0, 1, 0,
  0, 1, 1, 0, 0
];

var composer_1 = {};
composer_1.width = 6;
composer_1.height = 9;
composer_1.bitmap = [
  0, 0, 0, 1, 1, 0,
  0, 0, 1, 1, 1, 0,
  0, 1, 1, 1, 1, 0,
  0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 1, 0
];

var composer_2 = {};
composer_2.width = 7;
composer_2.height = 9;
composer_2.bitmap = [
  0, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 1, 1, 1, 0, 0,
  0, 1, 1, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 0
];

var composer_3 = {};
composer_3.width = 7;
composer_3.height = 9;
composer_3.bitmap = [
  0, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0
];

var composer_4 = {};
composer_4.width = 7;
composer_4.height = 9;
composer_4.bitmap = [
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 1, 1, 1, 0,
  0, 0, 1, 1, 1, 1, 0,
  0, 1, 1, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0
];

var composer_6 = {};
composer_6.width = 7;
composer_6.height = 9;
composer_6.bitmap = [
  0, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0
];

var composer_8 = {};
composer_8.width = 7;
composer_8.height = 9;
composer_8.bitmap = [
  0, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0
];

var composer_Dash = {};
composer_Dash.width = 5;
composer_Dash.height = 9;
composer_Dash.bitmap = [
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  1, 1, 1, 1, 1,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0
];

var composer_Hash = {};
composer_Hash.width = 8;
composer_Hash.height = 9;
composer_Hash.bitmap = [
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 0, 1, 1, 0, 0,
  0, 1, 1, 0, 1, 1, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 0,
  0, 1, 1, 0, 1, 1, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 0,
  0, 1, 1, 0, 1, 1, 0, 0,
  0, 1, 1, 0, 1, 1, 0, 0
];

var composer_Dot = {};
composer_Dot.width = 5;
composer_Dot.height = 9;
composer_Dot.bitmap = [
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  1, 1, 0, 0, 0,
  1, 1, 0, 0, 0
];

var composer_Cursor = {};
composer_Cursor.width = 2;
composer_Cursor.height = 11;
composer_Cursor.bitmap = [
  1, 0,
  1, 0,
  1, 0,
  1, 0,
  1, 0,
  1, 0,
  1, 0,
  1, 0,
  1, 0,
  1, 0,
  1, 0
];

var composer_c = {};
composer_c.width = 6;
composer_c.height = 9;
composer_c.bitmap = [
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 0, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0,
  1, 1, 0, 1, 1, 0,
  0, 1, 1, 1, 0, 0
];

var composer_d = {};
composer_d.width = 7;
composer_d.height = 9;
composer_d.bitmap = [
  0, 0, 0, 0, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 1, 0
];

var composer_e = {};
composer_e.width = 7;
composer_e.height = 9;
composer_e.bitmap = [
  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 1, 1, 1, 1, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0
];

var composer_f = {};
composer_f.width = 5;
composer_f.height = 9;
composer_f.bitmap = [
  0, 0, 1, 1, 0,
  0, 1, 1, 0, 0,
  1, 1, 1, 1, 0,
  0, 1, 1, 0, 0,
  0, 1, 1, 0, 0,
  0, 1, 1, 0, 0,
  0, 1, 1, 0, 0,
  0, 1, 1, 0, 0,
  0, 1, 1, 0, 0
];

var composer_g = {};
composer_g.width = 7;
composer_g.height = 11;
composer_g.bitmap = [
  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0
];

var composer_a = {};
composer_a.width = 7;
composer_a.height = 9;
composer_a.bitmap = [
  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 1, 0
];

var composer_b = {};
composer_b.width = 7;
composer_b.height = 9;
composer_b.bitmap = [
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 1, 1, 1, 0, 0
];

var composer_O = {};
composer_O.width = 7;
composer_O.height = 8;
composer_O.bitmap = [
  0, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0
];

var composer_p = {};
composer_p.width = 7;
composer_p.height = 9;
composer_p.bitmap = [
  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0
];

var composer_t = {};
composer_t.width = 4;
composer_t.height = 8;
composer_t.bitmap = [
  1, 1, 0, 0,
  1, 1, 0, 0,
  1, 1, 1, 0,
  1, 1, 0, 0,
  1, 1, 0, 0,
  1, 1, 0, 0,
  1, 1, 0, 0,
  0, 1, 1, 0
];

var composer_i = {};
composer_i.width = 3;
composer_i.height = 8;
composer_i.bitmap = [
  1, 1, 0,
  0, 0, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0
];

var composer_o = {};
composer_o.width = 6;
composer_o.height = 8;
composer_o.bitmap = [
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 0, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 1, 1, 0,
  0, 1, 1, 1, 0, 0
];

var composer_n = {};
composer_n.width = 6;
composer_n.height = 8;
composer_n.bitmap = [
  0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 0, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 1, 1, 0,
  1, 1, 0, 1, 1, 0
];

var composer_s = {};
composer_s.width = 5;
composer_s.height = 8;
composer_s.bitmap = [
  0, 0, 0, 0, 0,
  0, 0, 0, 0, 0,
  0, 1, 1, 1, 0,
  1, 1, 0, 0, 0,
  1, 1, 1, 0, 0,
  0, 1, 1, 1, 0,
  0, 0, 1, 1, 0,
  1, 1, 1, 0, 0
];

var composer_P = {};
composer_P.width = 7;
composer_P.height = 9;
composer_P.bitmap = [
  1, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 1, 1, 1, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 0, 0, 0
];

var composer_l = {};
composer_l.width = 3;
composer_l.height = 9;
composer_l.bitmap = [
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0,
  1, 1, 0
];

var composer_y = {};
composer_y.width = 7;
composer_y.height = 11;
composer_y.bitmap = [
  0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  1, 1, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 0, 1, 1, 0,
  0, 1, 1, 1, 1, 0, 0
];

var composer_Space = {};
composer_Space.width = 4;
composer_Space.height = 0;

var composerFont = [];
composerFont['1'] = composer_1;
composerFont['2'] = composer_2;
composerFont['3'] = composer_3;
composerFont['4'] = composer_4;
composerFont['6'] = composer_6;
composerFont['8'] = composer_8;
composerFont['-'] = composer_Dash;
composerFont['#'] = composer_Hash;
composerFont['.'] = composer_Dot;
composerFont['|'] = composer_Cursor;
composerFont['c'] = composer_c;
composerFont['d'] = composer_d;
composerFont['e'] = composer_e;
composerFont['f'] = composer_f;
composerFont['g'] = composer_g;
composerFont['a'] = composer_a;
composerFont['b'] = composer_b;
composerFont['O'] = composer_O;
composerFont['p'] = composer_p;
composerFont['t'] = composer_t;
composerFont['i'] = composer_i;
composerFont['o'] = composer_o;
composerFont['n'] = composer_n;
composerFont['s'] = composer_s;
composerFont['P'] = composer_P;
composerFont['l'] = composer_l;
composerFont['y'] = composer_y;
composerFont[' '] = composer_Space;

var notesRemainingFont = [];
notesRemainingFont['0'] = notesRemaining_0;
notesRemainingFont['1'] = notesRemaining_1;
notesRemainingFont['2'] = notesRemaining_2;
notesRemainingFont['3'] = notesRemaining_3;
notesRemainingFont['4'] = notesRemaining_4;
notesRemainingFont['5'] = notesRemaining_5;
notesRemainingFont['6'] = notesRemaining_6;
notesRemainingFont['7'] = notesRemaining_7;
notesRemainingFont['8'] = notesRemaining_8;
notesRemainingFont['9'] = notesRemaining_9;
