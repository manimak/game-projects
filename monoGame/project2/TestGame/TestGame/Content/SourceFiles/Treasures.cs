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
        Vector3 position;
        Boolean flaged = false;
        public Treasures(Stage theStage, string label, string meshFile, int x, int z)
            : base(theStage, label, meshFile)
        { initWall(x, z); }

        public Vector3 Position
        {
            get { return position; }
        }
        public Boolean Flaged
        {
            set { flaged = value; }
            get { return flaged; }
        }


        /// <summary>
        /// Shared constructor intialization code.
        /// </summary>
        /// <param name="offsetX"> center brick's x position</param>
        /// <param name="offsetZ"> center brick's z position</param>
        private void initWall(int X, int Z)
        {
            isCollidable = false;
            int spacing = stage.Terrain.Spacing;
            Terrain terrain = stage.Terrain;

            addObject(new Vector3(X * spacing, terrain.surfaceHeight(X, Z), Z * spacing), Vector3.Up, 0.0f);
            position = new Vector3(X * spacing, terrain.surfaceHeight(X, Z), Z * spacing);


        }
    }
}
