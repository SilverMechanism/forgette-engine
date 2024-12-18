This is an old 2D engine I mostly worked on in ~2023 (with some small updates in 2024) because I was interested in isomemtric games and a pre-rendered sprites workflow. 

It has no dependencies / libraries besides luajit, fmod, and sqlite / sqlitecpp. It runs on DirectX 11. It uses C++20 modules. 

You can modify definitions in the sqlite db and make some scripts using lua. Lugus is the lua script that auto-generates lua interop code.

It's pretty messy (and features some would-be-temporary hacks, workarounds, and defines for certain things) and I moved on from it to a new engine project learn 3D with DiligentGraphics but it was an excellent way to learn DirectX and how to build game systems from the ground up.
