# URoboViz

Unreal plugin for robot visualisation using ROS connecting with WebSockets. This plugin supports controlling robots built on SkeletalMeshActor and switching physics with an external physics engine [Mujoco](https://mujoco.org/).

## Overview
https://user-images.githubusercontent.com/64316740/162844451-ef9468df-f4e2-42d1-8d4f-78cd1200f979.mp4

## Plugin Dependencies
- [UROSBridge](git@github.com:robcog-iai/UROSBridge.git)
- [UUtils](git@github.com:robcog-iai/UUtils.git)

## ROS Dependencies
- [mujoco_sim](https://github.com/HoangGiang93/mujoco_sim)

## Features
- Combine photorealistic graphics from [Unreal Engine](https://www.unrealengine.com/en-US/) with advanced physics engine from [Mujoco](https://mujoco.org/) in runtime
- Spawning and Destroying objects in Mujoco world using Overlap Events from Unreal Engine in runtime
- Robot control using ROS as middleware (see [mujoco_sim](https://github.com/HoangGiang93/mujoco_sim))
