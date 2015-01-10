/*
  libSDL2pp - C++11 bindings/wrapper for SDL2
  Copyright (C) 2014 Dmitry Marakasov <amdmi3@amdmi3.ru>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include <vector>

#include <SDL2pp/Config.hh>

#include <SDL2/SDL_surface.h>
#ifdef SDL2PP_WITH_IMAGE
#	include <SDL2/SDL_image.h>
#endif

#include <SDL2pp/Surface.hh>
#include <SDL2pp/Exception.hh>
#ifdef SDL2PP_WITH_IMAGE
#	include <SDL2pp/RWops.hh>
#endif

namespace SDL2pp {

Surface::Surface(SDL_Surface* surface) : surface_(surface) {
}

Surface::Surface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	if ((surface_ = SDL_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask)) == nullptr)
		throw Exception("SDL_CreateRGBSurface failed");
}

Surface::Surface(void* pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	if ((surface_ = SDL_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask)) == nullptr)
		throw Exception("SDL_CreateRGBSurfaceFrom failed");
}

#ifdef SDL2PP_WITH_IMAGE
Surface::Surface(RWops& rwops) {
	if ((surface_ = IMG_Load_RW(rwops.Get(), 0)) == nullptr)
		throw Exception("IMG_Load_RW failed");
}

Surface::Surface(const std::string& path) {
	if ((surface_ = IMG_Load(path.c_str())) == nullptr)
		throw Exception("IMG_Load failed");
}
#endif

Surface::~Surface() {
	if (surface_ != nullptr)
		SDL_FreeSurface(surface_);
}

Surface::Surface(Surface&& other) noexcept : surface_(other.surface_) {
	other.surface_ = nullptr;
}

Surface& Surface::operator=(Surface&& other) noexcept {
	if (&other == this)
		return *this;
	if (surface_ != nullptr)
		SDL_FreeSurface(surface_);
	surface_ = other.surface_;
	other.surface_ = nullptr;
	return *this;
}

SDL_Surface* Surface::Get() const {
	return surface_;
}

Surface Surface::Convert(const SDL_PixelFormat& format) {
	SDL_Surface* surface = SDL_ConvertSurface(surface_, &format, 0);
	if (surface == nullptr)
		throw Exception("SDL_ConvertPixels failed");
	return surface;
}

Surface Surface::Convert(Uint32 pixel_format) {
	SDL_Surface* surface = SDL_ConvertSurfaceFormat(surface_, pixel_format, 0);
	if (surface == nullptr)
		throw Exception("SDL_ConvertPixels failed");
	return surface;
}

void Surface::Blit(const Optional<Rect>& srcrect, Surface& dst, const Rect& dstrect) {
	SDL_Rect tmpdstrect = dstrect; // 4th argument is non-const; does it modify rect?
	if (SDL_BlitSurface(surface_, srcrect ? &*srcrect : nullptr, dst.Get(), &tmpdstrect) != 0)
		throw Exception("SDL_BlitSurface failed");
}

void Surface::BlitScaled(const Optional<Rect>& srcrect, Surface& dst, const Optional<Rect>& dstrect) {
	SDL_Rect tmpdstrect; // 4th argument is non-const; does it modify rect?
	if (dstrect)
		tmpdstrect = *dstrect;
	if (SDL_BlitScaled(surface_, srcrect ? &*srcrect : nullptr, dst.Get(), dstrect ? &tmpdstrect : nullptr) != 0)
		throw Exception("SDL_BlitScaled failed");
}

Surface::LockHandle Surface::Lock() {
	return LockHandle(this);
}

Rect Surface::GetClipRect() const {
	SDL_Rect rect;
	SDL_GetClipRect(surface_, &rect);
	return Rect(rect);
}

Uint32 Surface::GetColorKey() const {
	Uint32 key;
	if (SDL_GetColorKey(surface_, &key) != 0)
		throw Exception("SDL_GetColorKey failed");
	return key;
}

Uint8 Surface::GetAlphaMod() const {
	Uint8 alpha;
	if (SDL_GetSurfaceAlphaMod(surface_, &alpha) != 0)
		throw Exception("SDL_GetSurfaceAlphaMod failed");
	return alpha;
}

SDL_BlendMode Surface::GetBlendMode() const {
	SDL_BlendMode blendMode;
	if (SDL_GetSurfaceBlendMode(surface_, &blendMode) != 0)
		throw Exception("SDL_GetSurfaceBlendMode failed");
	return blendMode;
}

void Surface::GetColorMod(Uint8& r, Uint8& g, Uint8& b) const {
	if (SDL_GetSurfaceColorMod(surface_, &r, &g, &b) != 0)
		throw Exception("SDL_GetSurfaceColorMod failed");
}

void Surface::SetClipRect(const Optional<Rect>& rect) {
	if (SDL_SetClipRect(surface_, rect ? &*rect : nullptr) != 0)
		throw Exception("SDL_SetClipRect failed");
}

void Surface::SetColorKey(int flag, Uint32 key) {
	if (SDL_SetColorKey(surface_, flag, key) != 0)
		throw Exception("SDL_SetColorKey failed");
}

void Surface::SetAlphaMod(Uint8 alpha) {
	if (SDL_SetSurfaceAlphaMod(surface_, alpha) != 0)
		throw Exception("SDL_SetSurfaceAlphaMod failed");
}

void Surface::SetBlendMode(SDL_BlendMode blendMode) {
	if (SDL_SetSurfaceBlendMode(surface_, blendMode) != 0)
		throw Exception("SDL_SetSurfaceBlendMode failed");
}

void Surface::SetColorMod(Uint8 r, Uint8 g, Uint8 b) {
	if (SDL_SetSurfaceColorMod(surface_, r, g, b) != 0)
		throw Exception("SDL_SetSurfaceColorMod failed");
}

void Surface::SetRLE(int flag) {
	if (SDL_SetSurfaceRLE(surface_, flag) != 0)
		throw Exception("SDL_SetSurfaceRLE failed");
}

void Surface::FillRect(const Optional<Rect>& rect, Uint32 color) {
	if (SDL_FillRect(surface_, rect ? &*rect : nullptr, color) != 0)
		throw Exception("SDL_FillRect failed");
}

void Surface::FillRects(const Rect* rects, int count, Uint32 color) {
	std::vector<SDL_Rect> sdl_rects;
	sdl_rects.reserve(count);
	for (const Rect* r = rects; r != rects + count; ++r)
		sdl_rects.emplace_back(*r);

	if (SDL_FillRects(surface_, sdl_rects.data(), sdl_rects.size(), color) != 0)
		throw Exception("SDL_FillRects failed");
}

int Surface::GetWidth() const {
	return surface_->w;
}

int Surface::GetHeight() const {
	return surface_->h;
}

Point Surface::GetSize() const {
	return Point(surface_->w, surface_->h);
}

}