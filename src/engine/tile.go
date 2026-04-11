package engine

import (
	"encoding/json"
	"image/color"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/storage"
)

const TileSize = 32

type TileCoord struct {
	X int32
	Y int32
}

type Tile struct {
	TileType            int32
	IsSolid             bool
	SpawnableMonsterIDs []int16
	ImgURI              fyne.URI

	Color  color.Color
	Coords TileCoord
}

func (t Tile) MarshalJSON() ([]byte, error) {
	type Alias Tile
	var uriStr string
	if t.ImgURI != nil {
		uriStr = t.ImgURI.String()
	}

	var colorVal interface{}
	if t.Color != nil {
		r, g, b, a := t.Color.RGBA()
		colorVal = struct {
			R uint8 `json:"r"`
			G uint8 `json:"g"`
			B uint8 `json:"b"`
			A uint8 `json:"a"`
		}{uint8(r >> 8), uint8(g >> 8), uint8(b >> 8), uint8(a >> 8)}
	}

	return json.Marshal(&struct {
		ImgURI string      `json:"ImgURI"`
		Color  interface{} `json:"Color"`
		Alias
	}{
		ImgURI: uriStr,
		Color:  colorVal,
		Alias:  (Alias)(t),
	})
}

func (t *Tile) UnmarshalJSON(data []byte) error {
	type Alias Tile
	aux := &struct {
		ImgURI string `json:"ImgURI"`
		Color  struct {
			R uint8 `json:"r"`
			G uint8 `json:"g"`
			B uint8 `json:"b"`
			A uint8 `json:"a"`
		} `json:"Color"`
		*Alias
	}{
		Alias: (*Alias)(t),
	}

	if err := json.Unmarshal(data, &aux); err != nil {
		return err
	}

	if aux.ImgURI != "" {
		if u, err := storage.ParseURI(aux.ImgURI); err == nil {
			t.ImgURI = u
		}
	}

	t.Color = color.NRGBA{R: aux.Color.R, G: aux.Color.G, B: aux.Color.B, A: aux.Color.A}
	return nil
}
