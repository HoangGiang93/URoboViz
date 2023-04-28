# URoboViz

Unreal plugin for robot visualisation using ROS connecting with WebSockets. This plugin supports controlling robots built on SkeletalMeshActor and switching physics with an external physics engine [MuJoCo](https://mujoco.org/).

## Overview
https://user-images.githubusercontent.com/64316740/235268851-fa43c940-2173-424b-9d42-6485c9b80f42.mp4

## Features
- Combine photorealistic graphics from [Unreal Engine](https://www.unrealengine.com/en-US/) with advanced physics engine from [MuJoCo](https://mujoco.org/) in runtime
- Spawning and Destroying objects in MuJoCo world using Overlap Events from Unreal Engine in runtime
- Robot control using ROS as middleware
- Run on both latest version of Unreal Engine 4.27 and 5.0.0

## Installation
To run this plugin properly, some dependencies need to be included. From Unreal side I use [UROSBridge](https://github.com/robcog-iai/UROSBridge) for ROS Communication and [UUtils](https://github.com/robcog-iai/UUtils) for coordination exchange between Unreal world and ROS world. To import robot from FBX format, I implemented an add-on for Blender [urdf_importer](https://github.com/HoangGiang93/urdf_importer), which can be used to convert robot from URDF to FBX format with textures. The robot can be controlled using physics engine of [MuJoCo](https://mujoco.org/) with ROS Interface from [mujoco_sim](https://github.com/HoangGiang93/mujoco_sim). See [Wiki](https://github.com/HoangGiang93/URoboViz/wiki) for more information.

## Tutorials
The tutorials can be found in [Wiki](https://github.com/HoangGiang93/URoboViz/wiki)
