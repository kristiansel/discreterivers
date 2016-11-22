#ifndef TEST_H
#define TEST_H

// want to write something like

// Dependency matrix for main game loop events
// T = This tick, P = Previous tick, E = External
//
//              Input   GUI     Player      AI      Physics     Macroworld  Graphics    Sound   |
// ============================================================================================================================
// Input        E,P     P                                                                       | Get input and change input states
//                                                                                              |
// GUI          T       P                                                                       | Close menus etc, move stuff
//                                                                                              |
// Player       T               P                                                               | Move player, do action etc
//                                                                                              |
// AI                                       P                                                   | Update surroundings and act
//                                                                                              |
// Physics                      T           T       T                                           | Apply move forces/hit tests etc
//                                                                                              |
// Macroworld                   T           T       T           T                               | Update the surroundings based on macroworld
//                                                                                              |
// Graphics                     T           T       T                       P                   |
//                                                                                              |
// Sound                        T           T       T                                   ?       |
// ===============================================================================================================================


// How should systems communicate?
// for example physics has calculated a bunch of transforms sorted by gameobject ID
// how can the graphics objects be updated

// const sortedarray<physTransform> &physTransforms;
// updateSortedByObjID(physTransforms, gfxTransforms);

// When a new game object is added:
GameObjectHandle // just an ID
createGameObject(PosCompt,)

#endif // TEST_H
