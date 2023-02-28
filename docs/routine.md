# Routine for TTool usage
* For now the ML integration is not considered.

U: user input/action
TT: what's fired in ttool program

- (1) U:: I want to detect the tool I want -> input start
  - (2) TT:: acces the camera frame
  - (3) TT:: start mask processing
- (4) U:: to move the tool around
  - (5) TT:: it runs tsegment
  - (6) TT:: visualize the mask result
- (7) U:: confirm the mask is correct

- (8) U:: input initial pose
- (9) U:: input the type of tool
  - (10) TT:: it runs tslet (IN: mask, initial pose, mesh model)
  - (11) TT:: visualize the result of the pose (overlaying object)

- (12) U:: confirm the pose is correct