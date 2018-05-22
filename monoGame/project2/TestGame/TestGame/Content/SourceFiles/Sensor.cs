

#region Using Statements
using System;
using System.IO;  // needed for trace()'s fout
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
#endregion

namespace AGMGSKv9
{

    /// <summary>
    /// A model that moves.  
    /// Has three Cameras:  first, follow, above.
    /// Camera agentCamera references the currently used camera {first, follow, above}
    /// Follow camera shows the MovableMesh from behind and up.
    /// Above camera looks down on avatar.
    /// The agentCamera (active camera) is updated by the avatar's Update().
    /// 
    /// 1/25/2012 last changed
    /// </summary>
    public class Sensor : MovableModel3D
    {
        private KeyboardState oldKeyboardState;
        protected Object3D sensorObject = null;
        protected Boolean collision = false;  


        public Sensor(Stage stage, string label, Vector3 position, Vector3 orientAxis,
           float radians, string meshFile)
           : base(stage, label, meshFile)
        {
            // create an Object3D for this agent
            sensorObject = addObject(position, orientAxis, radians ,new Vector3(10,10,10));
            isCollidable = true; // this turn the sensores off
        }

        public bool Collision
        {
            set { collision = value; }
            get { return collision; }
        }

        // Properties  

        public Object3D AgentObject
        {
            get { return sensorObject; }
        }

        // Methods

        public override string ToString()
        {
            return sensorObject.Name;
        }


        public override void Update(GameTime gameTime)
        {
            
            sensorObject.updateMovableObject();
            base.Update(gameTime);
            if (isCollidable)
            {
                foreach (Object3D obj3d in stage.Collidable)
                {
                    if (Vector3.Distance(sensorObject.Translation, obj3d.Translation) > 200)
                    {
                        collision = false;
                    }
                    else
                    {
                        collision = true;
                        break;
                    }

                }
            }


        }

    }
}
