package engine

import "image/color"

const TileSize = 32

type TileCoord struct {
	X int32
	Y int32
}

type Tile struct {
	TileType            int32
	IsSolid             bool
	SpawnableMonsterIDs []int16

	Color  color.Color
	Coords TileCoord
}
