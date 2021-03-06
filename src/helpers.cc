#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "helpers.h"

namespace sdl {

// Helper for formatting error exceptions
Handle<Value> ThrowSDLException(const char* name) {
  return ThrowException(MakeSDLException(name));
}

Local<Value> MakeSDLException(const char* name) {
  return Exception::Error(String::Concat(
    String::Concat(String::New(name), String::New(": ")),
    String::New(SDL_GetError())
  ));
}

// Wrap/Unwrap Surface

static Persistent<ObjectTemplate> surface_template_;

Handle<Value> GetSurfaceFlags(Local<String> name, const AccessorInfo& info) {
  SDL_Surface* surface = UnwrapSurface(info.Holder());
  return Number::New(surface->flags);
}
Handle<Value> GetSurfaceFormat(Local<String> name, const AccessorInfo& info) {
  HandleScope scope;
  SDL_Surface* surface = UnwrapSurface(info.Holder());
  return scope.Close(WrapPixelFormat(surface->format));
}
Handle<Value> GetSurfaceWidth(Local<String> name, const AccessorInfo& info) {
  SDL_Surface* surface = UnwrapSurface(info.Holder());
  return Number::New(surface->w);
}
Handle<Value> GetSurfaceHeight(Local<String> name, const AccessorInfo& info) {
  SDL_Surface* surface = UnwrapSurface(info.Holder());
  return Number::New(surface->h);
}
Handle<Value> GetSurfacePitch(Local<String> name, const AccessorInfo& info) {
  SDL_Surface* surface = UnwrapSurface(info.Holder());
  return Number::New(surface->pitch);
}
Handle<Value> GetSurfaceRect(Local<String> name, const AccessorInfo& info) {
  HandleScope scope;
  SDL_Surface* surface = UnwrapSurface(info.Holder());
  return scope.Close(WrapRect(&surface->clip_rect));
}

Handle<ObjectTemplate> MakeSurfaceTemplate() {
  HandleScope handle_scope;

  Handle<ObjectTemplate> result = ObjectTemplate::New();
  result->SetInternalFieldCount(1);

  // Add accessors for some of the fields of the surface.
  result->SetAccessor(String::NewSymbol("flags"), GetSurfaceFlags);
  result->SetAccessor(String::NewSymbol("format"), GetSurfaceFormat);
  result->SetAccessor(String::NewSymbol("w"), GetSurfaceWidth);
  result->SetAccessor(String::NewSymbol("h"), GetSurfaceHeight);
  result->SetAccessor(String::NewSymbol("pitch"), GetSurfacePitch);
  result->SetAccessor(String::NewSymbol("clip_rect"), GetSurfaceRect);

  // Again, return the result through the current handle scope.
  return handle_scope.Close(result);
}

Handle<Object> WrapSurface(SDL_Surface* surface) {
  // Handle scope for temporary handles.
  HandleScope handle_scope;

  // Fetch the template for creating JavaScript http request wrappers.
  // It only has to be created once, which we do on demand.
  if (surface_template_.IsEmpty()) {
    Handle<ObjectTemplate> raw_template = MakeSurfaceTemplate();
    surface_template_ = Persistent<ObjectTemplate>::New(raw_template);
  }
  Handle<ObjectTemplate> templ = surface_template_;

  // Create an empty http request wrapper.
  Handle<Object> result = templ->NewInstance();

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  Handle<External> request_ptr = External::New(surface);

  // Store the request pointer in the JavaScript wrapper.
  result->SetInternalField(0, request_ptr);

  // Return the result through the current handle scope.  Since each
  // of these handles will go away when the handle scope is deleted
  // we need to call Close to let one, the result, escape into the
  // outer handle scope.
  return handle_scope.Close(result);
}

SDL_Surface* UnwrapSurface(Handle<Object> obj) {
  Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<SDL_Surface*>(ptr);
}

// Wrap/Unwrap Rect

static Persistent<ObjectTemplate> rect_template_;

Handle<Value> GetRectX(Local<String> name, const AccessorInfo& info) {
  SDL_Rect* rect = UnwrapRect(info.Holder());
  return Number::New(rect->x);
}
Handle<Value> GetRectY(Local<String> name, const AccessorInfo& info) {
  SDL_Rect* rect = UnwrapRect(info.Holder());
  return Number::New(rect->y);
}
Handle<Value> GetRectW(Local<String> name, const AccessorInfo& info) {
  SDL_Rect* rect = UnwrapRect(info.Holder());
  return Number::New(rect->w);
}
Handle<Value> GetRectH(Local<String> name, const AccessorInfo& info) {
  SDL_Rect* rect = UnwrapRect(info.Holder());
  return Number::New(rect->h);
}

Handle<ObjectTemplate> MakeRectTemplate() {
  HandleScope handle_scope;

  Handle<ObjectTemplate> result = ObjectTemplate::New();
  result->SetInternalFieldCount(1);

  // Add accessors for some of the fields of the rect.
  result->SetAccessor(String::NewSymbol("x"), GetRectX);
  result->SetAccessor(String::NewSymbol("y"), GetRectY);
  result->SetAccessor(String::NewSymbol("w"), GetRectW);
  result->SetAccessor(String::NewSymbol("h"), GetRectH);

  // Again, return the result through the current handle scope.
  return handle_scope.Close(result);
}

Handle<Object> WrapRect(SDL_Rect* rect) {
  // Handle scope for temporary handles.
  HandleScope handle_scope;

  // Fetch the template for creating JavaScript http request wrappers.
  // It only has to be created once, which we do on demand.
  if (rect_template_.IsEmpty()) {
    Handle<ObjectTemplate> raw_template = MakeRectTemplate();
    rect_template_ = Persistent<ObjectTemplate>::New(raw_template);
  }
  Handle<ObjectTemplate> templ = rect_template_;

  // Create an empty http request wrapper.
  Handle<Object> result = templ->NewInstance();

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  Handle<External> request_ptr = External::New(rect);

  // Store the request pointer in the JavaScript wrapper.
  result->SetInternalField(0, request_ptr);

  // Return the result through the current handle scope.  Since each
  // of these handles will go away when the handle scope is deleted
  // we need to call Close to let one, the result, escape into the
  // outer handle scope.
  return handle_scope.Close(result);
}

SDL_Rect* UnwrapRect(Handle<Object> obj) {
  Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<SDL_Rect*>(ptr);
}

// Wrap/Unwrap PixelFormat

static Persistent<ObjectTemplate> pixelformat_template_;

Handle<Value> GetFormatBits(Local<String> name, const AccessorInfo& info) {
  SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
  return Number::New(format->BitsPerPixel);
}
Handle<Value> GetFormatBytes(Local<String> name, const AccessorInfo& info) {
  SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
  return Number::New(format->BytesPerPixel);
}
Handle<Value> GetFormatColorkey(Local<String> name, const AccessorInfo& info) {
  SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
  return Number::New(format->colorkey);
}
Handle<Value> GetFormatAlpha(Local<String> name, const AccessorInfo& info) {
  SDL_PixelFormat* format = UnwrapPixelFormat(info.Holder());
  return Number::New(format->alpha);
}

Handle<ObjectTemplate> MakePixelFormatTemplate() {
  HandleScope handle_scope;

  Handle<ObjectTemplate> result = ObjectTemplate::New();
  result->SetInternalFieldCount(1);

  // Add accessors for some of the fields of the pixelformat.
  result->SetAccessor(String::NewSymbol("bitsPerPixel"), GetFormatBits);
  result->SetAccessor(String::NewSymbol("bytesPerPixel"), GetFormatBytes);
  result->SetAccessor(String::NewSymbol("colorkey"), GetFormatColorkey);
  result->SetAccessor(String::NewSymbol("alpha"), GetFormatAlpha);

  // Again, return the result through the current handle scope.
  return handle_scope.Close(result);
}

Handle<Object> WrapPixelFormat(SDL_PixelFormat* pixelformat) {
  // Handle scope for temporary handles.
  HandleScope handle_scope;

  // Fetch the template for creating JavaScript http request wrappers.
  // It only has to be created once, which we do on demand.
  if (pixelformat_template_.IsEmpty()) {
    Handle<ObjectTemplate> raw_template = MakePixelFormatTemplate();
    pixelformat_template_ = Persistent<ObjectTemplate>::New(raw_template);
  }
  Handle<ObjectTemplate> templ = pixelformat_template_;

  // Create an empty http request wrapper.
  Handle<Object> result = templ->NewInstance();

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  Handle<External> request_ptr = External::New(pixelformat);

  // Store the request pointer in the JavaScript wrapper.
  result->SetInternalField(0, request_ptr);

  // Return the result through the current handle scope.  Since each
  // of these handles will go away when the handle scope is deleted
  // we need to call Close to let one, the result, escape into the
  // outer handle scope.
  return handle_scope.Close(result);
}

SDL_PixelFormat* UnwrapPixelFormat(Handle<Object> obj) {
  Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<SDL_PixelFormat*>(ptr);
}

// Wrap/Unwrap Joystick

static Persistent<ObjectTemplate> joystick_template_;

//Handle<Value> GetJoystickFlags(Local<String> name, const AccessorInfo& info) {
//  SDL_Joystick* joystick = UnwrapJoystick(info.Holder());
//  return Number::New(joystick->flags);
//}
//Handle<Value> GetJoystickFormat(Local<String> name, const AccessorInfo& info) {
//  HandleScope scope;
//  SDL_Joystick* joystick = UnwrapJoystick(info.Holder());
//  return scope.Close(WrapPixelFormat(joystick->format));
//}
//Handle<Value> GetJoystickWidth(Local<String> name, const AccessorInfo& info) {
//  SDL_Joystick* joystick = UnwrapJoystick(info.Holder());
//  return Number::New(joystick->w);
//}
//Handle<Value> GetJoystickHeight(Local<String> name, const AccessorInfo& info) {
//  SDL_Joystick* joystick = UnwrapJoystick(info.Holder());
//  return Number::New(joystick->h);
//}
//Handle<Value> GetJoystickPitch(Local<String> name, const AccessorInfo& info) {
//  SDL_Joystick* joystick = UnwrapJoystick(info.Holder());
//  return Number::New(joystick->pitch);
//}
//Handle<Value> GetJoystickRect(Local<String> name, const AccessorInfo& info) {
//  HandleScope scope;
//  SDL_Joystick* joystick = UnwrapJoystick(info.Holder());
//  return scope.Close(WrapRect(&joystick->clip_rect));
//}

Handle<ObjectTemplate> MakeJoystickTemplate() {
  HandleScope handle_scope;

  Handle<ObjectTemplate> result = ObjectTemplate::New();
  result->SetInternalFieldCount(1);

  // Add accessors for some of the fields of the joystick.
//  result->SetAccessor(String::NewSymbol("flags"), GetJoystickFlags);
//  result->SetAccessor(String::NewSymbol("format"), GetJoystickFormat);
//  result->SetAccessor(String::NewSymbol("w"), GetJoystickWidth);
//  result->SetAccessor(String::NewSymbol("h"), GetJoystickHeight);
//  result->SetAccessor(String::NewSymbol("pitch"), GetJoystickPitch);
//  result->SetAccessor(String::NewSymbol("clip_rect"), GetJoystickRect);

  // Again, return the result through the current handle scope.
  return handle_scope.Close(result);
}

Handle<Object> WrapJoystick(SDL_Joystick* joystick) {
  // Handle scope for temporary handles.
  HandleScope handle_scope;

  // Fetch the template for creating JavaScript http request wrappers.
  // It only has to be created once, which we do on demand.
  if (joystick_template_.IsEmpty()) {
    Handle<ObjectTemplate> raw_template = MakeJoystickTemplate();
    joystick_template_ = Persistent<ObjectTemplate>::New(raw_template);
  }
  Handle<ObjectTemplate> templ = joystick_template_;

  // Create an empty http request wrapper.
  Handle<Object> result = templ->NewInstance();

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  Handle<External> request_ptr = External::New(joystick);

  // Store the request pointer in the JavaScript wrapper.
  result->SetInternalField(0, request_ptr);

  // Return the result through the current handle scope.  Since each
  // of these handles will go away when the handle scope is deleted
  // we need to call Close to let one, the result, escape into the
  // outer handle scope.
  return handle_scope.Close(result);
}

SDL_Joystick* UnwrapJoystick(Handle<Object> obj) {
  Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<SDL_Joystick*>(ptr);
}

// Wrap/Unwrap Font

static Persistent<ObjectTemplate> font_template_;

//Handle<Value> GetFontFlags(Local<String> name, const AccessorInfo& info) {
//  TTF_Font* font = UnwrapFont(info.Holder());
//  return Number::New(font->flags);
//}
//Handle<Value> GetFontFormat(Local<String> name, const AccessorInfo& info) {
//  HandleScope scope;
//  TTF_Font* font = UnwrapFont(info.Holder());
//  return scope.Close(WrapPixelFormat(font->format));
//}
//Handle<Value> GetFontWidth(Local<String> name, const AccessorInfo& info) {
//  TTF_Font* font = UnwrapFont(info.Holder());
//  return Number::New(font->w);
//}
//Handle<Value> GetFontHeight(Local<String> name, const AccessorInfo& info) {
//  TTF_Font* font = UnwrapFont(info.Holder());
//  return Number::New(font->h);
//}
//Handle<Value> GetFontPitch(Local<String> name, const AccessorInfo& info) {
//  TTF_Font* font = UnwrapFont(info.Holder());
//  return Number::New(font->pitch);
//}
//Handle<Value> GetFontRect(Local<String> name, const AccessorInfo& info) {
//  HandleScope scope;
//  TTF_Font* font = UnwrapFont(info.Holder());
//  return scope.Close(WrapRect(&font->clip_rect));
//}

Handle<ObjectTemplate> MakeFontTemplate() {
  HandleScope handle_scope;

  Handle<ObjectTemplate> result = ObjectTemplate::New();
  result->SetInternalFieldCount(1);

  // Add accessors for some of the fields of the font.
//  result->SetAccessor(String::NewSymbol("flags"), GetFontFlags);
//  result->SetAccessor(String::NewSymbol("format"), GetFontFormat);
//  result->SetAccessor(String::NewSymbol("w"), GetFontWidth);
//  result->SetAccessor(String::NewSymbol("h"), GetFontHeight);
//  result->SetAccessor(String::NewSymbol("pitch"), GetFontPitch);
//  result->SetAccessor(String::NewSymbol("clip_rect"), GetFontRect);

  // Again, return the result through the current handle scope.
  return handle_scope.Close(result);
}

Handle<Object> WrapFont(TTF_Font* font) {
  // Handle scope for temporary handles.
  HandleScope handle_scope;

  // Fetch the template for creating JavaScript http request wrappers.
  // It only has to be created once, which we do on demand.
  if (font_template_.IsEmpty()) {
    Handle<ObjectTemplate> raw_template = MakeFontTemplate();
    font_template_ = Persistent<ObjectTemplate>::New(raw_template);
  }
  Handle<ObjectTemplate> templ = font_template_;

  // Create an empty http request wrapper.
  Handle<Object> result = templ->NewInstance();

  // Wrap the raw C++ pointer in an External so it can be referenced
  // from within JavaScript.
  Handle<External> request_ptr = External::New(font);

  // Store the request pointer in the JavaScript wrapper.
  result->SetInternalField(0, request_ptr);

  // Return the result through the current handle scope.  Since each
  // of these handles will go away when the handle scope is deleted
  // we need to call Close to let one, the result, escape into the
  // outer handle scope.
  return handle_scope.Close(result);
}

TTF_Font* UnwrapFont(Handle<Object> obj) {
  Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
  void* ptr = field->Value();
  return static_cast<TTF_Font*>(ptr);
}


char* BufferData(Buffer *b) {
  return Buffer::Data(b->handle_);
}

size_t BufferLength(Buffer *b) {
  return Buffer::Length(b->handle_);
}

char* BufferData(Local<Object> buf_obj) {
  return Buffer::Data(buf_obj);
}

size_t BufferLength(Local<Object> buf_obj) {
  return Buffer::Length(buf_obj);
}

} // node_sdl
