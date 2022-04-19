# URoboViz

Unreal plugin for robot visualisation using ROS connecting with WebSockets. This plugin supports controlling robots built on SkeletalMeshActor and switching physics with an external physics engine [Mujoco](https://mujoco.org/).

## Overview
https://user-images.githubusercontent.com/64316740/162844451-ef9468df-f4e2-42d1-8d4f-78cd1200f979.mp4

## Features
- Combine photorealistic graphics from [Unreal Engine](https://www.unrealengine.com/en-US/) with advanced physics engine from [Mujoco](https://mujoco.org/) in runtime
- Spawning and Destroying objects in Mujoco world using Overlap Events from Unreal Engine in runtime
- Robot control using ROS as middleware
- Run on both latest version of Unreal Engine 4.27 and 5.0.0

## Installation
To run this plugin properly, some dependencies need to be included. From Unreal side I use [UROSBridge](https://github.com/robcog-iai/UROSBridge) for ROS Communication and [UUtils](https://github.com/robcog-iai/UUtils) for coordinatation exchange between Unreal world and ROS world. To import robot from FBX format, I implemented an add-on for Blender [urdf_importer](https://github.com/HoangGiang93/urdf_importer), which can be used to convert robot from URDF to FBX format with textures. The robot can be controlled using physics engine of [Mujoco](https://mujoco.org/) with ROS Interface from [mujoco_sim](https://github.com/HoangGiang93/mujoco_sim). See [Wiki](https://github.com/HoangGiang93/URoboViz/wiki) for more information.
