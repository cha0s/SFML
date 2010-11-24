	/* rbSFML - Copyright (c) 2010 Henrik Valter Vogelius Hansson - groogy@groogy.se
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented;
 *    you must not claim that you wrote the original software.
 *    If you use this software in a product, an acknowledgment
 *    in the product documentation would be appreciated but
 *    is not required.
 *
 * 2. Altered source versions must be plainly marked as such,
 *    and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any
 *    source distribution.
 */
 
#include "RenderWindow.hpp"
#include "main.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

VALUE globalRenderWindowClass;

/* External classes */
extern VALUE globalRenderTargetModule;
extern VALUE globalWindowClass;
extern VALUE globalDrawableModule;
extern VALUE globalShaderClass;

static void RenderWindow_Free( sf::RenderWindow *anObject )
{
	delete anObject;
}

/* call-seq:
 *   render_window.draw( drawable )
 *   render_window.draw( drawable, shader )
 *
 * Draw an object into the target with a shader.
 *
 * This function draws anything that inherits from the SFML::Drawable base class (SFML::Sprite, SFML::Shape,
 * SFML::Text, or even your own derived classes). The shader alters the way that the pixels are processed right before
 * being written to the render target.
 */
static VALUE RenderWindow_Draw( int argc, VALUE *args, VALUE self )
{
	sf::RenderWindow *object = NULL;
	Data_Get_Struct( self, sf::RenderWindow, object );
	switch( argc )
	{
		case 2:
		{
			VALIDATE_CLASS( args[0], globalDrawableModule, "object" );
			VALIDATE_CLASS( args[1], globalShaderClass, "shader" );
			sf::Drawable *drawable = NULL;
			Data_Get_Struct( args[0], sf::Drawable, drawable );
			sf::Shader *shader = NULL;
			Data_Get_Struct( args[1], sf::Shader, shader );
			object->Draw( *drawable, *shader );
			break;
		}
		case 1:
		{
			VALIDATE_CLASS( args[0], globalDrawableModule, "object" );
			sf::Drawable *drawable = NULL;
			Data_Get_Struct( args[0], sf::Drawable, drawable );
			object->Draw( *drawable );
			break;
		}
		default:
			rb_raise( rb_eArgError, "Expected 1 or 2 arguments but was given %d", argc );
	}
	return Qnil;
}

/* call-seq:
 *   Window.new()											-> render_window
 *   Window.new( mode, title, style = SFML::Style::Default, settings = SFML::ContextSettings.new )	-> render_window
 *
 * Construct a new window.
 *
 * The first form of new doesn't actually create the visual window, use the other form of new or call 
 * SFML::Window#create to do so.
 *
 * The second form of new creates the window with the size and pixel depth defined in mode. An optional style can be passed 
 * to customize the look and behaviour of the window (borders, title bar, resizable, closable, ...). If style contains 
 * Style::Fullscreen, then mode must be a valid video mode.
 *
 * The fourth parameter is an optional structure specifying advanced OpenGL context settings such as antialiasing, 
 * depth-buffer bits, etc. You shouldn't care about these parameters for a regular usage of the graphics module.
 */
static VALUE RenderWindow_New( int argc, VALUE *args, VALUE aKlass )
{
	sf::RenderWindow *object = new sf::RenderWindow();
	VALUE rbData = Data_Wrap_Struct( aKlass, 0, RenderWindow_Free, object );
	rb_obj_call_init( rbData, argc, args );
	return rbData;
}

void Init_RenderWindow( void )
{
/* SFML namespace which contains the classes of this module. */
	VALUE sfml = rb_define_module( "SFML" );
/* Window that can serve as a target for 2D drawing.
 *
 * SFML::RenderWindow is the main class of the Graphics module.
 *
 * It defines an OS window that can be painted using the other classes of the graphics module.
 *
 * SFML::RenderWindow is derived from SFML::Window, thus it inherits all its features: mouse/keyboard/joystick input, 
 * events, window handling, OpenGL rendering, etc. See the documentation of SFML::Window for a more complete description
 * of all these features and code samples.
 *
 * On top of that, SFML::RenderWindow adds more features related to 2D drawing with the graphics module (see its base 
 * class SFML::RenderTarget for more details). Here is a typical rendering / event loop with a SFML::RenderWindow:
 *
 *   # Declare and create a new render-window
 *   window = SFML::RenderWindow.new( SFML::VideoMode.new( 800, 600 ), "SFML window" )
 *
 *   # Limit the framerate to 60 frames per second (this step is optional)
 *   window.framerateLimit = 60
 *
 *   # The main loop - ends as soon as the window is closed
 *   while window.open?
 *     # Event processing
 *     while event = window.getEvent
 *       # Request for closing the window
 *       if event.type == SFML::Event::Closed)
 *            window.close
 *       end
 *     end
 *
 *     # Clear the whole window before rendering a new frame
 *     window.clear
 *
 *     # Draw some sprites / shapes / texts
 *     window.draw( sprite )  # sprite is a SFML::Sprite
 *     window.draw( shape )   # shape is a SFML::Shape
 *     window.draw( text )    # text is a SFML::Text
 *
 *     # End the current frame and display its contents on screen
 *     window.display
 *   end
 *
 * Like SFML::Window, SFML::RenderWindow is still able to render direct OpenGL stuff. It is even possible to mix 
 * together OpenGL calls and regular SFML drawing commands. When doing so, make sure that OpenGL states are not messed 
 * up by calling the saveGLStates / restoreGLStates functions.
 *
 *   # Create the render window
 *   window = SFML::RenderWindow.new( SFML::VideoMode( 800, 600 ), "SFML OpenGL" )
 *
 *   # Create a sprite and a text to display
 *   sprite = SFML::Sprite.new
 *   text = SFML::Sprite.new
 *   # ...
 *
 *   # Perform OpenGL initializations
 *   glMatrixMode( GL_PROJECTION )
 *   # ...
 *
 *   # Start the rendering loop
 *   while window.open?
 *     # Process events
 *     # ...
 *
 *     # Draw a background sprite
 *     window.saveGLStates
 *     window.draw( sprite )
 *     window.restoreGLStates
 *
 *     # Draw a 3D object using OpenGL
 *     glBegin( GL_QUADS )
 *       glVertex3f( ... )
 *       # ...
 *     glEnd()
 *
 *     # Draw text on top of the 3D object
 *     window.saveGLStates
 *     window.draw( text )
 *     window.restoreGLStates
 *
 *     # Finally, display the rendered frame on screen
 *     window.display
 *   end
 *
 */
 	globalRenderWindowClass	= rb_define_class_under( sfml, "RenderWindow", globalWindowClass );
	rb_include_module( globalRenderWindowClass, globalRenderTargetModule );
	
	// Class methods
	rb_define_singleton_method( globalRenderWindowClass, "new", RenderWindow_New, -1 );
	
	// Instance methods
	rb_define_method( globalRenderWindowClass, "draw", RenderWindow_Draw, -1 );
}
