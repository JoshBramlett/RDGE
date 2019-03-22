from cli_wrappers import aseprite
from utils import rwops

#
#
#
#
#
#
# {
#   "name": "dominic",
#   "type": "npc",
#   "animations": [
#     "battle.aseprite",
#     "idle_blink.aseprite",
#     "run.aseprite",
#     "walk.aseprite"
#   ],
#   "static_frames": [
#     "dead.aseprite",
#     "gestures.aseprite"
#   ]
# }


# import_path {path}/{sheet_type}/{sheet_name}/{sprite_name}/animations/{animation_name}
# import_path {path}/npcs/town/dominic/animations/walk
# export_path {path}/{sheet_type}/{sheet_name}/{sprite_name}/{animation_name}.json


class NPCSheet:
    __animation_format = '{path}/{sheet_type}/{sheet_name}/{sprite_name}/animations/{animation_name}'
    __animations_key = 'animations'
    __static_frames_key = 'static_frames'

    def __init__(self, raw_asset_path, export_path, sheet_definition):
        self.__raw_asset_path = raw_asset_path
        self.__export_path = export_path
        self.__definition = sheet_definition

    def export(self):
        sprites = self.__definition['sprites']
        for sprite in sprites:
            sprite_name = sprite['name']
            animations = sprite[self.__animations_key]
            static_frames = sprite[self.__static_frames_key]

            for animation in animations:

