package engine

import (
	"image/color"
	"math"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/storage"
	"fyne.io/fyne/v2/widget"
)

var gridColor = color.NRGBA{150, 150, 150, 60}
var gridFocusColor = color.NRGBA{100, 100, 255, 255}

const renderPadding = 5

/// IMPORTANT Zooming in and out has SHIT performance

type mapRenderer struct {
	canvas           *MapViewport
	objs             []fyne.CanvasObject
	rectPool         map[TileCoord]*canvas.Rectangle
	imgPool          map[TileCoord]*canvas.Image
	inactiveImgPool  map[fyne.Resource][]*canvas.Image
	inactiveRectPool []*canvas.Rectangle
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
	ResCache      map[string]fyne.Resource

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
		ResCache:      make(map[string]fyne.Resource),
	}

	v.ExtendBaseWidget(v)
	return v
}

func (v *MapViewport) LoadResource(uri fyne.URI) fyne.Resource {
	if uri == nil || uri.String() == "" {
		return nil
	}
	if res, ok := v.ResCache[uri.String()]; ok {
		return res
	}
	res, _ := storage.LoadResourceFromURI(uri)
	v.ResCache[uri.String()] = res
	return res
}

func (v *MapViewport) CreateRenderer() fyne.WidgetRenderer {
	rend := &mapRenderer{
		canvas:           v,
		objs:             []fyne.CanvasObject{},
		rectPool:         make(map[TileCoord]*canvas.Rectangle),
		imgPool:          make(map[TileCoord]*canvas.Image),
		inactiveImgPool:  make(map[fyne.Resource][]*canvas.Image),
		inactiveRectPool: []*canvas.Rectangle{},
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
}

func (r *mapRenderer) Layout(size fyne.Size) {
	r.objs = r.objs[:0]

	visibleStartX := int(math.Floor(float64(-r.canvas.OffsetX / r.canvas.TileSize)))
	visibleStartY := int(math.Floor(float64(-r.canvas.OffsetY / r.canvas.TileSize)))
	visibleEndX := int(math.Ceil(float64((size.Width - r.canvas.OffsetX) / r.canvas.TileSize)))
	visibleEndY := int(math.Ceil(float64((size.Height - r.canvas.OffsetY) / r.canvas.TileSize)))

	startX := visibleStartX - renderPadding
	startY := visibleStartY - renderPadding
	endX := visibleEndX + renderPadding
	endY := visibleEndY + renderPadding

	visibleCoords := make(map[TileCoord]bool)

	for x := startX; x < endX; x++ {
		for y := startY; y < endY; y++ {
			coord := TileCoord{X: int32(x), Y: int32(y)}

			if tileData, exists := r.canvas.MapData[coord]; exists {
				if tileData.Resource != nil || tileData.ImgURI != nil {
					res := tileData.Resource
					if res == nil {
						res = r.canvas.LoadResource(tileData.ImgURI)
						tileData.Resource = res
						r.canvas.MapData[coord] = tileData
					}

					img, inPool := r.imgPool[coord]
					if !inPool {
						/// Try to find an inactive image that ALREADY has this exact resource pointer
						if bucket, ok := r.inactiveImgPool[res]; ok && len(bucket) > 0 {
							img = bucket[len(bucket)-1]
							r.inactiveImgPool[res] = bucket[:len(bucket)-1]
						} else {
							/// Take any inactive image from any bucket and swap the resource
							for resPtr, fallbackBucket := range r.inactiveImgPool {
								if len(fallbackBucket) > 0 {
									img = fallbackBucket[len(fallbackBucket)-1]
									r.inactiveImgPool[resPtr] = fallbackBucket[:len(fallbackBucket)-1]
									img.Resource = res
									img.Refresh()
									break
								}
							}
						}

						if img == nil {
							img = canvas.NewImageFromResource(res)
							img.FillMode = canvas.ImageFillStretch
						}
						r.imgPool[coord] = img
					} else if img.Resource != res {
						img.Resource = res
						img.Refresh()
					}

					pixelX := float32(x)*r.canvas.TileSize + r.canvas.OffsetX
					pixelY := float32(y)*r.canvas.TileSize + r.canvas.OffsetY

					img.Move(fyne.NewPos(pixelX, pixelY))
					img.Resize(fyne.NewSize(r.canvas.TileSize, r.canvas.TileSize))
					img.Show()
					r.objs = append(r.objs, img)
					visibleCoords[coord] = true
				} else {
					rect, in_pool := r.rectPool[coord]
					if !in_pool {
						if len(r.inactiveRectPool) > 0 {
							rect = r.inactiveRectPool[len(r.inactiveRectPool)-1]
							r.inactiveRectPool = r.inactiveRectPool[:len(r.inactiveRectPool)-1]
						} else {
							rect = canvas.NewRectangle(color.Transparent)
							rect.StrokeWidth = 1
						}
						r.rectPool[coord] = rect
					}

					rect.StrokeColor = tileData.Color

					pixelX := float32(x)*r.canvas.TileSize + r.canvas.OffsetX
					pixelY := float32(y)*r.canvas.TileSize + r.canvas.OffsetY

					rect.Move(fyne.NewPos(pixelX, pixelY))
					rect.Resize(fyne.NewSize(r.canvas.TileSize, r.canvas.TileSize))
					rect.Show()

					r.objs = append(r.objs, rect)
					visibleCoords[coord] = true
				}
			}
		}
	}

	for coord, img := range r.imgPool {
		if !visibleCoords[coord] {
			img.Hide()
			if img.Resource != nil {
				resPtr := img.Resource
				r.inactiveImgPool[resPtr] = append(r.inactiveImgPool[resPtr], img)
			}
			delete(r.imgPool, coord)
		}
	}
	for coord, rect := range r.rectPool {
		if !visibleCoords[coord] {
			rect.Hide()
			r.inactiveRectPool = append(r.inactiveRectPool, rect)
			delete(r.rectPool, coord)
		}
	}

	lineIndex := 0
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

	/// Culling
	if r.canvas.TileSize >= 10 {
		for x := startX; x <= endX; x++ {
			line := getLine()
			pixelX := float32(x)*r.canvas.TileSize + r.canvas.OffsetX

			line.Position1 = fyne.NewPos(pixelX, 0)
			line.Position2 = fyne.NewPos(pixelX, size.Height)
			line.Show()

			r.objs = append(r.objs, line)
		}

		for y := startY; y <= endY; y++ {
			line := getLine()
			pixelY := float32(y)*r.canvas.TileSize + r.canvas.OffsetY

			line.Position1 = fyne.NewPos(0, pixelY)
			line.Position2 = fyne.NewPos(size.Width, pixelY)
			line.Show()

			r.objs = append(r.objs, line)
		}
	}

	for i := lineIndex; i < len(r.gridLines); i++ {
		r.gridLines[i].Hide()
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
		cursor.Show()
		r.objs = append(r.objs, cursor)
	}

	for i := cursorIndex; i < len(r.selectionCursors); i++ {
		r.selectionCursors[i].Hide()
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
	// Calculate world position under the mouse before zooming
	worldX := (e.Position.X - m.OffsetX) / m.TileSize
	worldY := (e.Position.Y - m.OffsetY) / m.TileSize

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

	// Adjust offsets so the world position stays under the mouse cursor
	m.OffsetX = e.Position.X - (worldX * m.TileSize)
	m.OffsetY = e.Position.Y - (worldY * m.TileSize)

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
	}

	m.Refresh()
}

func (m *MapViewport) FocusLost()                {}
func (m *MapViewport) FocusGained()              {}
func (m *MapViewport) TypedRune(rune)            {}
func (m *MapViewport) TypedKey(e *fyne.KeyEvent) {}

type TappableImage struct {
	widget.Icon
	OnTap func()
}

func NewTappableImage(res fyne.Resource, onTap func()) *TappableImage {
	t := &TappableImage{OnTap: onTap}
	t.ExtendBaseWidget(t)
	t.SetResource(res)
	return t
}

func (t *TappableImage) Tapped(_ *fyne.PointEvent) {
	if t.OnTap != nil {
		t.OnTap()
	}
}

func (t *TappableImage) Cursor() desktop.Cursor {
	return desktop.PointerCursor
}

func (t *TappableImage) MinSize() fyne.Size {
	return fyne.NewSize(64, 64)
}
