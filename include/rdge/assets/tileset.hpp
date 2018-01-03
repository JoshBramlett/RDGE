//! \headerfile <rdge/assets/tileset.hpp>
//! \author Josh Bramlett
//! \version 0.0.10
//! \date 11/26/2017

#pragma once

#include <rdge/core.hpp>
#include <rdge/assets/surface.hpp>

//! \namespace rdge Rainbow Drop Game Engine
namespace rdge {

{ "columns":12,
 "image":"overworld.png",
 "imageheight":194,
 "imagewidth":194,
 "margin":1,
 "name":"overworld_tileset",
 "spacing":0,
 "tilecount":144,
 "tileheight":16,
 "tiles":
    {
     "82":
        {
         "objectgroup":
            {
             "draworder":"index",
             "name":"",
             "objects":[
                    {
                     "height":6,
                     "id":1,
                     "name":"",
                     "rotation":0,
                     "type":"",
                     "visible":true,
                     "width":5,
                     "x":5,
                     "y":5
                    }],
             "opacity":1,
             "type":"objectgroup",
             "visible":true,
             "x":0,
             "y":0
            }
        }
    },
 "tilewidth":16,
 "type":"tileset"
}

{
  "name":"overworld_tileset",
  "type":"tileset",
  "image":"overworld.png",
  "margin":1,
  "spacing":0,
  "tilecount":144,
  "columns":12,
  "tileheight":16,
  "tilewidth":16,
  "tiledata": [
    {
      "index": 82,
      "name": "name_in_enum",
      "objects": [
        {
          "name": "",
          "type": "polygon",
          "usertype": "hitbox",
          "rotation":0,
          "verts": [
            {
              "x": 5,
              "y": 5,
            },
            {
              "x": 5,
              "y": 5,
            }
          ]
        },
        {
          "name": "",
          "type": "circle",
          "usertype": "hitbox",
          "x": 5,
          "y": 5,
          "radius":0
        }
      ],
      "properties": [
        {
          "name": "myprop",
          "type": "float",
          "value": 3.14
        }
      ]
    }
  ]
}

class Tileset
{
public:

public:
    struct tileset_tile
    {
        std::string name;
        screen_rect clip;
        tex_coords uv;
        math::vec2 size;
        math::vec2 origin;

        std::vector<tilemap::Object> objects;
        tilemap::PropertyCollection properties; //!< Custom variable type property collection
    };

    std::string name;
    int32 asset_id;

    size_t rows = 0;
    size_t cols = 0;
    size_t spacing = 0;
    size_t margin = 0;

    math::uivec2 tile_size;
    tileset_tile* tiles;
    size_t count;

    tilemap::PropertyCollection properties; //!< Custom variable type property collection
    Surface surface; //!< Pixel data of the sprite sheet
};

} // namespace rdge
