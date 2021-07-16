#pragma once
#include <widget/Widget.hpp>


namespace rack {
namespace widget {


/** Caches its children's draw() result to a framebuffer image.
When dirty, its children will be re-rendered on the next call to step().
*/
struct FramebufferWidget : Widget {
	struct Internal;
	Internal* internal;

	/** Set this to true to re-render the children to the framebuffer the next time it is drawn */
	bool dirty = true;
	bool bypassed = false;
	float oversample = 1.0;
	/** Redraw when the world offset of the FramebufferWidget changes its fractional value. */
	bool dirtyOnSubpixelChange = true;

	FramebufferWidget();
	~FramebufferWidget();
	void setDirty(bool dirty = true);
	int getImageHandle();
	NVGLUframebuffer* getFramebuffer();
	math::Vec getFramebufferSize();

	void step() override;
	/** Draws the framebuffer to the NanoVG scene, re-rendering it if necessary.
	*/
	void draw(const DrawArgs& args) override;
	/** Re-renders the framebuffer, re-creating it if necessary.
	Handles oversampling (if >1) by rendering to a temporary (larger) framebuffer and then downscaling it to the main persistent framebuffer.
	*/
	void render(math::Vec scale, math::Vec offsetF);
	/** Initializes the current GL context and draws children to it.
	*/
	virtual void drawFramebuffer();

	void onDirty(const DirtyEvent& e) override;
	void onContextCreate(const ContextCreateEvent& e) override;
	void onContextDestroy(const ContextDestroyEvent& e) override;
};


} // namespace widget
} // namespace rack
