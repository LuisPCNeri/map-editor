package engine

import (
	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/widget"
)

type mapRenderer struct {
	canvas *MapViewport
	objs   []fyne.CanvasObject
	pool   map[TileCoord]*canvas.Rectangle
}

type MapViewport struct {
	widget.BaseWidget

	MapData  map[TileCoord]Tile
	OffsetX  float32
	OffsetY  float32
	Zoom     float32
	TileSize float32
}

func NewMapViewport() *MapViewport {
	v := &MapViewport{
		MapData:  make(map[TileCoord]Tile),
		TileSize: float32(TileSize),
		OffsetX:  0,
		OffsetY:  0,
		Zoom:     0,
	}

	v.ExtendBaseWidget(v)
	return v
}

func (v *MapViewport) CreateRenderer() fyne.WidgetRenderer {
	rend := &mapRenderer{
		canvas: v,
		objs:   []fyne.CanvasObject{},
		pool:   make(map[TileCoord]*canvas.Rectangle),
	}

	return rend
}

func (r *mapRenderer) Destroy() {}

func (r *mapRenderer) Objects() []fyne.CanvasObject {
	return r.objs
}

func (r *mapRenderer) MinSize() fyne.Size {
	return fyne.NewSize(200, 200)
}

func (r *mapRenderer) Refresh() {
	canvas.Refresh(r.canvas)
}

func (r *mapRenderer) Layout(size fyne.Size) {
	r.objs = nil

	startX := int(-r.canvas.OffsetX / r.canvas.TileSize)
	startY := int(-r.canvas.OffsetY / r.canvas.TileSize)

	endX := startX + int(size.Width/r.canvas.TileSize)
	endY := startY + int(size.Height/r.canvas.TileSize)

	for x := startX; x < endX; x++ {
		for y := startY; y < endY; y++ {
			coord := TileCoord{X: int32(x), Y: int32(y)}

			if tileData, exists := r.canvas.MapData[coord]; exists {

				rect, in_pool := r.pool[coord]
				if !in_pool {
					rect = canvas.NewRectangle(tileData.Color)
					r.pool[coord] = rect
				}

				pixelX := float32(x)*r.canvas.TileSize + r.canvas.OffsetX
				pixelY := float32(y)*r.canvas.TileSize + r.canvas.OffsetY

				rect.Move(fyne.NewPos(pixelX, pixelY))
				rect.Resize(fyne.NewSize(r.canvas.TileSize, r.canvas.TileSize))

				r.objs = append(r.objs, rect)
			}
		}
	}
}
