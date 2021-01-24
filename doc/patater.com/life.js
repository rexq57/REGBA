////////
// Tweak Zone - You can tweak stuff here.
//
var display_fps = false;
var fps_cap = 10; // Set to negative to uncap (e.g. -1).
var wrap_around = true;
var width = 128;  // How many pixels in the field. Note that these
var height = 128; // dimensions must be power of two if wrap around is enabled.
//
////////

// Based on http://mrdoob.com/lab/javascript/webgl/glsl/01/
var fps;
var content;
var canvas;
var gl;
var compute_program;
var display_program;
var messages;
var vertex_buffer;
var texture_coord_buffer;
var textures = new Array();
var framebuffers = new Array();
var active_index = 0;
var inactive_index = 1;
var frames = 0;
var prev_time = new Date().getTime();
var dx = 1 / width;
var dy = 1 / height;
var mod_pixels;
var mod_pixels_dirty;
var mod_pixels_uploaded = false;
var draw_happened = false;
var zero_pixels;
var mod_texture = 2;

// <http://paulirish.com/2011/requestanimationframe-for-smart-animating/>
requestAnimFrame = (function() {
  if (fps_cap > 0) {
    return function(callback, element) {
      window.setTimeout(callback, 1000 / fps_cap);
    };
  } else {
    return window.requestAnimationFrame ||
           window.webkitRequestAnimationFrame ||
           window.mozRequestAnimationFrame ||
           window.oRequestAnimationFrame ||
           window.msRequestAnimationFrame ||
           function(callback, element) {
             window.setTimeout(callback, 1000 / 60);
           };
  }
})();

function is_power_of_two(x)
{
  // Return true if x is an integer power of two.
  if (x > 0) {
    return (x & (x - 1)) == 0;
  } else {
    // Negative numbers aren't integer powers of two.
    return false;
  }
}


window['init'] = init;
function init() {
  var vertex_source = document.getElementById('vertex-shader').textContent;
  var compute_source = document.getElementById('compute').textContent;
  var display_source = document.getElementById('display').textContent;


  fps = document.getElementById('fps');
  content = document.getElementById('content');
  messages = document.getElementById('messages');
  canvas = document.getElementById('content-canvas');

  // Initialize WebGL
  try {
    gl = canvas.getContext('experimental-webgl', {depth: false});
  } catch (error) {}
  if (!gl) {
    alert('Experimental WebGL is not supported.');
    return;
  }

  // Create the programs.
  compute_program = create_program(vertex_source, compute_source);
  display_program = create_program(vertex_source, display_source);
  if (compute_program == null || display_program == null) {
    return;
  }

  // Obtain variable handles.
  gl.useProgram(compute_program);
  compute_program.position = gl.getAttribLocation(compute_program, 'position');
  gl.enableVertexAttribArray(compute_program.position);
  compute_program.a_uv = gl.getAttribLocation(compute_program, 'a_uv');
  gl.enableVertexAttribArray(compute_program.a_uv);
  compute_program.sampler = gl.getUniformLocation(compute_program, 'sampler');
  compute_program.mod_sampler = gl.getUniformLocation(
      compute_program, 'mod_sampler'
      );
  compute_program.dx = gl.getUniformLocation(compute_program, 'dx');
  compute_program.dy = gl.getUniformLocation(compute_program, 'dy');

  gl.useProgram(display_program);
  display_program.position = gl.getAttribLocation(display_program, 'position');
  gl.enableVertexAttribArray(display_program.position);
  display_program.a_uv = gl.getAttribLocation(display_program, 'a_uv');
  gl.enableVertexAttribArray(display_program.a_uv);
  display_program.sampler = gl.getUniformLocation(display_program, 'sampler');

  // Create the vertex buffer.
  vertex_buffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
  var vertex_coords =
      new Float32Array(
          [-1.0, -1.0, 0.0,
           1.0, -1.0, 0.0,
           -1.0, 1.0, 0.0,
           1.0, 1.0, 0.0]);
  gl.bufferData(gl.ARRAY_BUFFER, vertex_coords, gl.STATIC_DRAW);
  vertex_buffer.element_size = 3;
  vertex_buffer.count = 4;

  // Create the texture coordinates buffer.
  texture_coord_buffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, texture_coord_buffer);
  var texture_coords =
      new Float32Array(
          [0, 0,
           1, 0,
           0, 1,
           1, 1]);
  gl.bufferData(gl.ARRAY_BUFFER, texture_coords, gl.STATIC_DRAW);
  texture_coord_buffer.element_size = 2;
  texture_coord_buffer.count = 4;

  create_textures();

  display_message('Draw in the box.\n');
  canvas.addEventListener('mousedown', mousedown_canvas, false);
  canvas.addEventListener('mouseover', mouseover_canvas, false);

  if (display_fps) {
    setInterval(update_fps, 500);
  }
  animate();
}

function mouseover_canvas(event)
{
  canvas.style.cursor = 'url(8x8_block.png), default';
}

function mousedown_canvas(event)
{
  if (!draw_happened) {
    clear_messages();
    draw_happened = true;
  }
  var scale_x = canvas.width / width;
  var scale_y = canvas.height / height;

  var position_x = canvas.offsetLeft;
  var position_y = canvas.offsetTop;

  var x = (event.clientX - position_x + window.pageXOffset) / scale_x >> 0;
  var y = (event.clientY - position_y + window.pageYOffset) / scale_y >> 0;

  // Buffer the x and y position.
  set_pixel(mod_pixels, x, y, 0xff);
  set_pixel(mod_pixels, x + 1, y, 0xff);
  set_pixel(mod_pixels, x, y + 1, 0xff);
  set_pixel(mod_pixels, x + 1, y + 1, 0xff);
  mod_pixels_dirty = true;

  document.addEventListener('mousemove', canvas_mouseheld, false);
  document.addEventListener('mouseup', canvas_mouseup, false);
  event.preventDefault();
}

function canvas_mouseheld(event)
{
  var scale_x = canvas.width / width;
  var scale_y = canvas.height / height;

  var position_x = canvas.offsetLeft;
  var position_y = canvas.offsetTop;

  var x = (event.clientX - position_x + window.pageXOffset) / scale_x >> 0;
  var y = (event.clientY - position_y + window.pageYOffset) / scale_y >> 0;

  // Buffer the x and y position.
  set_pixel(mod_pixels, x, y, 0xff);
  set_pixel(mod_pixels, x + 1, y, 0xff);
  set_pixel(mod_pixels, x, y + 1, 0xff);
  set_pixel(mod_pixels, x + 1, y + 1, 0xff);
  mod_pixels_dirty = true;

  event.preventDefault();
}

function canvas_mouseup(event)
{
  document.removeEventListener('mousemove', canvas_mouseheld, false);
  document.removeEventListener('mouseup', canvas_mouseup, false);
}

function create_dead_population(alive, dead)
{
  var cells = new Array();
  // Generate dead population
  for (var i = 0; i < width; i++) {
    for (var j = 0; j < height; j++) {
      cells.push(0, 0, 0, 0);
    }
  }
  return cells;
}

function set_pixel(pixels, x, y, value)
{
  if (x < 0 || y < 0 || x > width || y > height) {
    return;
  }

  // 4 is the size of a pixel
  pixels[(y * width + x) * 4] = value;
  pixels[(y * width + x) * 4 + 1] = value;
  pixels[(y * width + x) * 4 + 2] = value;
  pixels[(y * width + x) * 4 + 3] = value;
}

function create_random_population(alive, dead)
{
  var cells = new Array();
  var density = 0.5; //Math.random() / 100;
  // Generate random population texture
  for (var i = 0; i < width; i++) {
    for (var j = 0; j < height; j++) {
      if (Math.random() > density) {
        // Dead
        cells.push(0, 0, 0, 0);
      } else {
        // Alive
        cells.push(0xff, 0xff, 0xff, 0xff);
      }
    }
  }
  return cells;
}

function create_texture(pixels)
{
  var texture = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, texture);
  gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
  // TODO We can probably get away with a simpler texture format
  // than RGBA UNSIGNED_BYTE, assuming that isn't the simplest
  // one.
  gl.texImage2D(
      gl.TEXTURE_2D, 0, gl.RGBA, width, height,
      0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array(pixels)
  );
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

  var do_wrap_around =
      wrap_around && is_power_of_two(width) && is_power_of_two(height);
  if (wrap_around && !do_wrap_around) {
    display_message('Please use power of two sized width and height if' +
                    'you want wrap around.');
  }

  if (do_wrap_around) {
    // NOTE: Unfortunately, we can't do wrap around textures that
    // aren't a power of two.
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
  } else {
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
  }
  return texture;
}

function create_framebuffer(texture)
{
  var framebuffer = gl.createFramebuffer();
  gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
  gl.framebufferTexture2D(
      gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0,
      gl.TEXTURE_2D, texture, 0
  );
  return framebuffer;
}

function create_program(vertex_source, fragment_source)
{
  var program = gl.createProgram();
  var vertex_shader = create_shader(vertex_source, gl.VERTEX_SHADER);
  var fragment_shader = create_shader(fragment_source, gl.FRAGMENT_SHADER);
  if (vertex_shader == null || fragment_shader == null) {
    return null;
  }

  gl.attachShader(program, vertex_shader);
  gl.attachShader(program, fragment_shader);

  gl.deleteShader(vertex_shader);
  gl.deleteShader(fragment_shader);

  gl.linkProgram(program);
  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    display_message(
        'Error ' +
        gl.getProgramParameter(display_program, gl.VALIDATE_STATUS) + '\n' +
        gl.getError()
    );
    return null;
  }
  return program;
}

function create_shader(source, type)
{
  var shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);

  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    display_message(
        'Error with ' +
        (type == gl.VERTEX_SHADER ? 'VERTEX' : 'FRAGMENT') + ' SHADER\n' +
        gl.getShaderInfoLog(shader)
    );
    return null;
  }

  return shader;
}

function create_textures()
{
  // Create compute textures
  var pixels = create_dead_population();
  textures[0] = create_texture(pixels);
  textures[1] = create_texture(pixels);

  // Create compute frame buffers
  framebuffers[0] = create_framebuffer(textures[0]);
  framebuffers[1] = create_framebuffer(textures[1]);

  // Create mod pixels
  mod_pixels = create_dead_population();
  zero_pixels = create_dead_population();
  textures[mod_texture] = create_texture(mod_pixels);
  mod_pixels_dirty = true;
}

function display_message(message_text)
{
  var message = document.createElement('p');
  message.textContent = message_text;
  messages.appendChild(message);
}

function clear_messages()
{
  messages.innerHTML = '';
}

function animate()
{
  requestAnimFrame(animate);
  compute();
  display();

  frames++;
}

function upload_mod_texture()
{
  if (mod_pixels_dirty) {
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    gl.texImage2D(
        gl.TEXTURE_2D, 0, gl.RGBA, width, height,
        0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array(mod_pixels)
    );
    // XXX I hope the old array gets garbage collected.

    mod_pixels = zero_pixels.slice();
    mod_pixels_uploaded = true;
    mod_pixels_dirty = false;
  } else if (mod_pixels_uploaded) {
    // Upload zero stuff
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    gl.texImage2D(
        gl.TEXTURE_2D, 0, gl.RGBA, width, height,
        0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array(mod_pixels)
    );
    // XXX I hope the old array gets garbage collected.
    mod_pixels_uploaded = false;
  }
}

function compute()
{
  if (!compute_program) return;
  gl.viewport(0, 0, width, height);

  gl.useProgram(compute_program);
  gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffers[inactive_index]);

  gl.bindBuffer(gl.ARRAY_BUFFER, texture_coord_buffer);
  gl.vertexAttribPointer(
      compute_program.a_uv,
      texture_coord_buffer.element_size,
      gl.FLOAT,
      false,
      0,
      0
  );

  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D, textures[active_index]);
  gl.uniform1i(compute_program.sampler, 0);
  gl.uniform1f(compute_program.dx, dx);
  gl.uniform1f(compute_program.dy, dy);
  var temp = active_index;
  active_index = inactive_index;
  inactive_index = temp;

  gl.activeTexture(gl.TEXTURE1);
  gl.bindTexture(gl.TEXTURE_2D, textures[mod_texture]);
  upload_mod_texture();
  gl.uniform1i(compute_program.mod_sampler, 1);

  gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
  gl.vertexAttribPointer(
      compute_program.position,
      vertex_buffer.element_size,
      gl.FLOAT,
      false,
      0,
      0
  );
  gl.drawArrays(gl.TRIANGLE_STRIP, 0, vertex_buffer.count);
  gl.flush();
}

function display()
{
  if (!display_program) return;
  gl.viewport(0, 0, canvas.width, canvas.height);

  gl.useProgram(display_program);
  gl.bindFramebuffer(gl.FRAMEBUFFER, null);

  gl.bindBuffer(gl.ARRAY_BUFFER, texture_coord_buffer);
  gl.vertexAttribPointer(
      display_program.a_uv,
      texture_coord_buffer.element_size,
      gl.FLOAT,
      false,
      0,
      0
  );
  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D, textures[active_index]);
  gl.uniform1i(display_program.sampler, 0);

  gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
  gl.vertexAttribPointer(
      display_program.position,
      vertex_buffer.element_size,
      gl.FLOAT,
      false,
      0,
      0
  );
  gl.drawArrays(gl.TRIANGLE_STRIP, 0, vertex_buffer.count);
  gl.flush();
}

function update_fps()
{
  var now = new Date().getTime();
  var fps_num = Math.round(1000 * frames / (now - prev_time));
  frames = 0;
  prev_time = now;
  fps.textContent = 'fps: ' + fps_num;
}

document.addEventListener('DOMContentLoaded', function () {
  init();
});
