package engine

import (
	"image/color"
	"math"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/widget"
)

var gridColor = color.NRGBA{150, 150, 150, 60}
var gridFocusColor = color.NRGBA{100, 100, 255, 255}

type mapRenderer struct {
	canvas           *MapViewport
	objs             []fyne.CanvasObject
	pool             map[TileCoord]*canvas.Rectangle
	selectionCursors []*canvas.Rectangle

	gridLines []*canvas.Line
}

type MapViewport struct {
	widget.BaseWidget

	MapData  map[TileCoord]Tile
	OffsetX  float32
	OffsetY  float32
	Zoom     float32
	TileSize float32

	SelectedTiles map[TileCoord]bool

	isDragging bool
}

func NewMapViewport() *MapViewport {
	v := &MapViewport{
		MapData:  make(map[TileCoord]Tile),
		TileSize: float32(TileSize),
		OffsetX:  0,
		OffsetY:  0,
		Zoom:     0,

		SelectedTiles: make(map[TileCoord]bool),
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
	r.Layout(r.canvas.Size())
	canvas.Refresh(r.canvas)
}

func (r *mapRenderer) Layout(size fyne.Size) {
	r.objs = nil
	lineIndex := 0

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
					rect = canvas.NewRectangle(color.Transparent)
					rect.StrokeColor = tileData.Color
					rect.StrokeWidth = 1
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

	getLine := func() *canvas.Line {
		if lineIndex >= len(r.gridLines) {
			newLine := canvas.NewLine(gridColor)
			newLine.StrokeWidth = 1
			r.gridLines = append(r.gridLines, newLine)
		}

		line := r.gridLines[lineIndex]
		lineIndex++
		return line
	}

	for x := startX; x <= endX; x++ {
		line := getLine()
		pixelX := float32(x)*r.canvas.TileSize + r.canvas.OffsetX

		line.Position1 = fyne.NewPos(pixelX, 0)
		line.Position2 = fyne.NewPos(pixelX, size.Height)

		r.objs = append(r.objs, line)
	}

	for y := startY; y <= endY; y++ {
		line := getLine()
		pixelY := float32(y)*r.canvas.TileSize + r.canvas.OffsetY

		line.Position1 = fyne.NewPos(0, pixelY)
		line.Position2 = fyne.NewPos(size.Width, pixelY)

		r.objs = append(r.objs, line)
	}

	cursorIndex := 0

	for coord := range r.canvas.SelectedTiles {

		if cursorIndex >= len(r.selectionCursors) {
			cursor := canvas.NewRectangle(color.Transparent)
			cursor.StrokeColor = gridFocusColor
			cursor.StrokeWidth = 1

			r.selectionCursors = append(r.selectionCursors, cursor)
		}

		cursor := r.selectionCursors[cursorIndex]
		cursorIndex++

		pixelX := float32(coord.X)*r.canvas.TileSize + r.canvas.OffsetX
		pixelY := float32(coord.Y)*r.canvas.TileSize + r.canvas.OffsetY

		cursor.Move(fyne.NewPos(pixelX, pixelY))
		cursor.Resize(fyne.NewSize(r.canvas.TileSize, r.canvas.TileSize))
		r.objs = append(r.objs, cursor)
	}
}

func (m *MapViewport) Dragged(e *fyne.DragEvent) {
	m.isDragging = true

	m.OffsetX += e.Dragged.DX
	m.OffsetY += e.Dragged.DY

	m.Refresh()
}

// / Required by interface ig
func (m *MapViewport) DragEnd() {
	m.Refresh()
}

func (m *MapViewport) Scrolled(e *fyne.ScrollEvent) {
	zoomSpeed := float32(2)

	if e.Scrolled.DY > 0 {
		m.TileSize += zoomSpeed
	} else if e.Scrolled.DY < 0 {
		m.TileSize -= zoomSpeed
	}

	if m.TileSize < 16 {
		m.TileSize = 16
	}
	if m.TileSize > 128 {
		m.TileSize = 128
	}

	m.Refresh()
}

func (m *MapViewport) MouseDown(e *desktop.MouseEvent) {
	fyne.CurrentApp().Driver().CanvasForObject(m).Focus(m)
	m.isDragging = false
}

func HandleLMBPress(m *MapViewport, e *desktop.MouseEvent) {
	gridX := int(math.Floor(float64((e.Position.X - m.OffsetX) / m.TileSize)))
	gridY := int(math.Floor(float64((e.Position.Y - m.OffsetY) / m.TileSize)))
	coord := TileCoord{int32(gridX), int32(gridY)}

	isSelectMultiple := (e.Modifier & fyne.KeyModifierShortcutDefault) != 0

	if isSelectMultiple {
		if m.SelectedTiles[coord] {
			delete(m.SelectedTiles, coord)
		} else {
			m.SelectedTiles[coord] = true
		}
	} else {
		if m.SelectedTiles[coord] && len(m.SelectedTiles) == 1 {
			m.SelectedTiles = make(map[TileCoord]bool)
		} else {
			m.SelectedTiles = make(map[TileCoord]bool)
			m.SelectedTiles[coord] = true
		}
	}
}

func (m *MapViewport) MouseUp(e *desktop.MouseEvent) {
	if m.isDragging {
		m.isDragging = false
		return
	}

	if e.Button == desktop.MouseButtonPrimary {
		HandleLMBPress(m, e)
		return
	}

	m.Refresh()
}

func (m *MapViewport) FocusLost()                {}
func (m *MapViewport) FocusGained()              {}
func (m *MapViewport) TypedRune(rune)            {}
func (m *MapViewport) TypedKey(e *fyne.KeyEvent) {}
