// Todo
{

    // TODO::Now
    {
      // # Implement keyboard/mouse input handling
      // # Get camera moving.
      // # Clean up all shader files for each chapter.
      // # Set option to have initial window query for screen size and start full-screen-windowed.
      // # Timer is wrong.
      // # On build failure, remove the binary from /bin/ so that we don't accidentally keep running an old version of the code if we don't notice build output.

      // # Double check that you can't store textures in the store array by the texture ID rather than the vertex attribute id.

      // According to DumpState, the viewport width and height are not being set in GLState::viewport. The same goes for the window state/aspect ratio on resize -- Viewport gets updated, but Window does not (within state cahce). Additionally, on startup (before resizing), the viewport state is not getting set. So window state is not getting set after a window-resize (but viewport is), and viewport is not getting set  _without_ a resize (but window is).

      // Split the StateDump to a) just getting the state dump tring in one function and b) a function that calls (a) as well as prints it. If you do this, you can get the state dump string in LogType::ltError messages and dump state every time there's an error. You'd be able to do the same in the glError check within the function wrappers.

      // Better debug messaging? https://www.khronos.org/opengl/wiki/Debug_Output
      // Paint "Graceful exit" green in terminal output?
    }

    // TODO::Eventually
    {
      // # Implement binary portion of PLY loader.
      // # Dont' query gl functions with glew, use a static fnptr member within each of the Impl_<> functions that can be referenced there: static fnptr glClear = GetTheFnPtr() ... then call it rather than the glew version
    }
}

// Before Pushing To Dev/Master
{
    // # Every previous chapter must work with newly implemented or altered functions, insofar as they are used in previous chapters.
    // # All [ cfarvin::<> ] tags must be addressed, removed, or moved to the "TODO" section above.
}

// Build All
cd C :\workspace\vermont_dreams\src\chapter_2&& build.bat&& ^
  cd C :\workspace\vermont_dreams\src\chapter_4&& build.bat&& ^
  cd C :\workspace\vermont_dreams\src\chapter_5&& build.bat&& ^
  cd                                              C
  :\workspace\vermont_dreams\src\chapter_6&&      build.bat

    // Run All
    cd C :\workspace\vermont_dreams\bin&& ^
  chapter_2.exe&& ^ chapter_4.exe&& ^ chapter_5.exe&& ^ chapter_6.exe
