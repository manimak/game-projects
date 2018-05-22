using System;
using System.IO; 
using System.Collections.Generic;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Input;


namespace AGMGSKv9
{

    /// <summary>

    /// </summary>

    public class Treasures : Model3D
    {

        public Treasures(Stage theStage, string label, string meshFile) : base(theStage, label, meshFile)
        {
            initWall(450, 460);  // origin of wall on terrain
        }

        public Treasures(Stage theStage, string label, string meshFile, int xOffset, int zOffset)
            : base(theStage, label, meshFile)
        { initWall(xOffset, zOffset); }

        /// <summary>
        /// Shared constructor intialization code.
        /// </summary>
        /// <param name="offsetX"> center brick's x position</param>
        /// <param name="offsetZ"> center brick's z position</param>
        private void initWall(int offsetX, int offsetZ)
        {
            isCollidable = false;
            int spacing = stage.Terrain.Spacing;
            Terrain terrain = stage.Terrain;
            int baseX = offsetX;
            int baseZ = offsetZ;
            // brick[x,z] vertex positions on terrain
            int[,] brick = {  {-3, -7}, {21, 20}, {32, 20}, {15, 20}};
            for (int i = 0; i < brick.GetLength(0); i++)
            {
                int xPos = brick[i, 0] + baseX;
                int zPos = brick[i, 1] + baseZ;
                addObject(new Vector3(xPos * spacing, terrain.surfaceHeight(xPos, zPos), zPos * spacing), Vector3.Up, 0.0f,new Vector3(2, 2, 2));
            }
            
        }
    }
}
