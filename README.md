# Motive-UE4
Motive Plugin for the Unreal Engine. Using the NatNet SDK to enable streaming of tracking data from Motive into Unreal.

# Installation

* Create new c++ First Person project
* Merge the "Content" folder with your project Content folder
* Copy the plugin "MotiveToUnity" to the "Plugins" folder of your project, if there is no "Plugins" folder, create on.
* Copy the NatNet .dll from the Third Party/lib folder to the same directory as the editor .exe. (I'm not sure why this is needed and should be fixed)
* When you start the Unreal Editor, the plugin should compile. If it fails, try Generating project files then rebuilding your project.

# Useage
* Data streaming should be enabled in motive.
* Add the MotiveReceiveExample blueprint to your scene for an example of how to handle incoming data.
* The ID parameter of the GetRigidbodyData blueprint node corresponds to the "User Data" value of a ridigid body in Motive.



