/*  
    Copyright (C) 2017 G. Michael Barnes
 
    The file Pack.cs is part of AGMGSKv9 a port and update of AGXNASKv8 from
    MonoGames 3.5 to MonoGames 3.6  

    AGMGSKv9 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#region Using Statements
using System;
using System.IO;  // needed for trace()'s fout
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;
#endregion

namespace AGMGSKv9 {

/// <summary>
/// Pack represents a "flock" of MovableObject3D's Object3Ds.
/// Usually the "player" is the leader and is set in the Stage's LoadContent().
/// With no leader, determine a "virtual leader" from the flock's members.
/// Model3D's inherited List<Object3D> instance holds all members of the pack.
/// 
/// 2/1/2016 last changed
/// </summary>
public class Pack : MovableModel3D {   
   private Object3D leader;
   private float lvlOfPacking = 1;
   private String percentagePacking = "levels of packing: 0%";
   private int differentPackinglvl = 0;
   private int nDogs;
   private KeyboardState oldKeyboardState;


        /// <summary>
        /// Construct a pack with an Object3D leader
        /// </summary>
        /// <param name="theStage"> the scene </param>
        /// <param name="label"> name of pack</param>
        /// <param name="meshFile"> model of a pack instance</param>
        /// <param name="xPos, zPos">  approximate position of the pack </param>
        /// <param name="aLeader"> alpha dog can be used for flock center and alignment </param>
        public Pack(Stage theStage, string label, string meshFile, int nDogs, int xPos, int zPos, Object3D theLeader)
      : base(theStage, label, meshFile) {
      this.nDogs = nDogs;
      isCollidable = true;
      random = new Random();
      leader = theLeader;
	  int spacing = stage.Spacing;
        
      if (leader != null)
      {
          xPos = (int)(leader.Translation.X/spacing);
          zPos = (int)(leader.Translation.Z/spacing);
      }
      // initial vertex offset of dogs around (xPos, zPos)
      int[,] position = { {0, 3}, {7, -40}, {-50, -2}, {-70, 4}, {5, 20}, { -100, 3 } };
      
	  for( int i = 0; i < nDogs ; i++)
      {
		int x = xPos + position[i, 0];
		int z = zPos + position[i, 1];
        float scale = (float)(0.5 + random.NextDouble());
		addObject( new Vector3(x * spacing, stage.surfaceHeight(x, z), z * spacing),
					  new Vector3(0, 1, 0), 0.0f,
					  new Vector3(scale, scale, scale));
	  }


   }



   /// <summary>
   /// Each pack member's orientation matrix will be updated.
   /// Distribution has pack of dogs moving randomly.  
   /// Supports leaderless and leader based "flocking" 
   /// </summary>      
   public override void Update(GameTime gameTime) {
        // if (leader == null) need to determine "virtual leader from members"
        KeyboardState keyboardState = Keyboard.GetState();
        if (keyboardState.IsKeyDown(Keys.P) && !oldKeyboardState.IsKeyDown(Keys.P))
        {
            if(differentPackinglvl == 0)
            {
                lvlOfPacking = 0.99f;
                percentagePacking = "levels of packing: 99%";
            }
            else if (differentPackinglvl == 1)
            {
                lvlOfPacking = 0.995f;
                percentagePacking = "levels of packing: 67%";
            }
            else if (differentPackinglvl == 2)
            {
                lvlOfPacking = 0.998f;
                percentagePacking = "levels of packing: 33%";
            }
            else if(differentPackinglvl == 3)
            {
                lvlOfPacking = 1f;
                percentagePacking = "levels of packing: 0%";
            }

            differentPackinglvl++;
            if(differentPackinglvl % 4 == 0)
            {
                differentPackinglvl = 0;
            }
                

        }
        stage.setInfo(10, percentagePacking);
        oldKeyboardState = keyboardState;
        float angle = 0.3f;
      foreach (Object3D obj in instance) {
                obj.Yaw = 0.0f;
                //obj.Yaw = leader.Yaw;
                // change direction 4 time a second  0.07 = 4/60
                //  if (percentage2 % 70 == 0)
                // {
                if ( random.NextDouble()< lvlOfPacking)
                {

                        if (random.NextDouble() < 0.1)
                        {
                            if (random.NextDouble() < 0.5) obj.Yaw -= angle; // turn left
                            else obj.Yaw += angle; // turn right
                        }
                }
                else
                {
                        
                        
                         
                        
                        foreach (Object3D otherObj in instance)
                        {
                            if (Vector3.Distance(obj.Translation, otherObj.Translation) < 2000)
                            {
                                if (random.NextDouble() < 0.5)
                                {
                                    obj.Yaw = 0.5f;

                                }
                            }
                            else
                            {
                                obj.turnToFace(leader.Translation );
                                
                            }



                        }


                }
                //  }
                // else
                // {
                //    obj.Yaw = leader.Yaw;
                // } 
     
                  
         
         obj.updateMovableObject();
         stage.setSurfaceHeight(obj);
      }
      base.Update(gameTime);  // MovableMesh's Update(); 
   }


   public Object3D Leader {
      get { return leader; }
      set { leader = value; }}

   }
}
