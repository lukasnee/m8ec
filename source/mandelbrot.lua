local SCREEN_WIDTH = 320
local SCREEN_HEIGHT = 240

local function get_color(iter, max_iter)
    if iter == MAX_ITER then
        return 0, 0, 0
    end
    local t = iter / max_iter
    local r = math.floor(9 * (1 - t) * t * t * t * 255)
    local g = math.floor(15 * (1 - t) * (1 - t) * t * t * 255)
    local b = math.floor(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255)
    return r, g, b
end

local function draw_mandelbrot(x_offset, y_offset, zoom, max_iter)
    for y = 0, SCREEN_HEIGHT - 1 do
        for x = 0, SCREEN_WIDTH - 1 do
            local cr = (x * zoom) - (SCREEN_WIDTH/2 * zoom) + x_offset
            local ci = (y * zoom) - (SCREEN_HEIGHT/2 * zoom) + y_offset
            local zr, zi = 0, 0
            local iter = 0
            while (zr * zr + zi * zi < 4) and (iter < max_iter) do
                local temp = zr * zr - zi * zi + cr
                zi = 2 * zr * zi + ci
                zr = temp
                iter = iter + 1
            end
            local r, g, b = get_color(iter, max_iter)
            ili9341_draw_pixel(r, g, b, x, y)
        end
    end
end

draw_mandelbrot(-0.75, 0.1, 0.01, 25)