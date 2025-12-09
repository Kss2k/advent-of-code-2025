file <- "input.txt"
lines <- readLines(file)
X <- stringr::str_split_fixed(lines, pattern = ",", n = 2) |>
  apply(MARGIN = 2, FUN = as.integer)
coordinates <- lapply(seq_len(NROW(X)), \(i) c(x = X[i, 1], y = X[i, 2]))

# Re-index
idx <- seq_len(NROW(X))

funcArea <- \(x, y) abs(y[, 1] - x[, 1] + 1) * abs(y[, 2] - x[, 2] + 1)
funcIdxArea <- \(xi, yi) funcArea(X[xi, ], X[yi, ])
areas <- outer(idx, idx, funcIdxArea)

# Task 1
cat("Answer Task 1: ", max(areas), "\n")

# Task 2
getLine <- function(x, y) {
  list(x = x, y = y, horizontal = x[2] == y[2], vertical = x[1] == y[1])
}

linesIntersect <- function(a, b) {
  if (a$horizontal && b$vertical) {
    ((a$x[1] <= b$x[1] && a$y[1] >= b$x[1]) ||
     (a$x[1] >= b$x[1] && a$y[1] <= b$x[1])) &&
    ((a$x[2] <= b$x[2] && a$x[2] >= b$y[2]) ||
     (a$x[2] >= b$x[2] && a$x[2] <= b$y[2]))
  } else if (a$vertical && b$horizontal) {
    linesIntersect(b, a)
  } else {
    FALSE
  }
}

Tiles <- list(vertical = list(), horizontal = list())
for (i in seq_len(NROW(X))) {
  j <- ifelse(i == 1, NROW(X), i - 1)
  x <- X[i, ]
  y <- X[j, ]

  line <- getLine(x, y)

  if (line$vertical) Tiles$vertical <- c(Tiles$vertical, list(line))
  if (line$horizontal) Tiles$horizontal <- c(Tiles$horizontal, list(line))
}

VerticalEdges <- lapply(Tiles$vertical, \(line) {
  list(
    x = line$x[1],
    ymin = min(line$x[2], line$y[2]),
    ymax = max(line$x[2], line$y[2])
  )
})

HorizontalEdges <- lapply(Tiles$horizontal, \(line) {
  list(
    y = line$x[2],
    xmin = min(line$x[1], line$y[1]),
    xmax = max(line$x[1], line$y[1])
  )
})

pointIsInTilesSingle <- function(point) {
  # horizontal ray intersection using a single ray starting outside the loop
  end   <- c(max(X[, 1]) + 1, point[2])
  hline <- getLine(point, end)

  counter <- 0
  for (vline in Tiles$vertical) {
    counter <- counter + as.integer(linesIntersect(hline, vline))
  }

  counter %% 2 == 1
}


pointInTiles <- function(points) {
  vapply(points, FUN.VALUE = logical(1L), FUN = pointIsInTilesSingle)
}
# pointInTiles(list(c(x = 9, y = 3)))

insideCache <- new.env(parent = emptyenv())
pointOnBoundary <- function(point) {
  for (hline in Tiles$horizontal) {
    if (point[2] != hline$x[2])
      next

    if (point[1] >= min(hline$x[1], hline$y[1]) &&
        point[1] <= max(hline$x[1], hline$y[1]))
      return(TRUE)
  }

  for (vline in Tiles$vertical) {
    if (point[1] != vline$x[1])
      next

    if (point[2] >= min(vline$x[2], vline$y[2]) &&
        point[2] <= max(vline$x[2], vline$y[2]))
      return(TRUE)
  }

  FALSE
}

pointInside <- function(point) {
  key <- paste(point[1], point[2], sep = ",")
  if (!exists(key, envir = insideCache, inherits = FALSE)) {
    assign(
      key,
      pointOnBoundary(point) || pointIsInTilesSingle(point),
      envir = insideCache
    )
  }
  get(key, envir = insideCache, inherits = FALSE)
}

rowBreaks <- sort(unique(X[, 2]))
rowIntervals <- lapply(seq_len(length(rowBreaks) - 1), \(i) {
  y <- rowBreaks[i]
  xs <- c()
  for (edge in VerticalEdges) {
    if (y >= edge$ymin && y < edge$ymax)
      xs <- c(xs, edge$x)
  }
  xs <- sort(xs)
  if (length(xs) >= 2) matrix(xs, ncol = 2, byrow = TRUE) else matrix(numeric(0), ncol = 2)
})

getRowIntervals <- function(y) {
  idx <- findInterval(y, rowBreaks, rightmost.closed = FALSE)
  if (idx == 0 || idx > length(rowIntervals))
    return(matrix(numeric(0), ncol = 2))
  rowIntervals[[idx]]
}

colBreaks <- sort(unique(X[, 1]))
columnIntervals <- lapply(seq_len(length(colBreaks) - 1), \(i) {
  x <- colBreaks[i]
  ys <- c()
  for (edge in HorizontalEdges) {
    if (x >= edge$xmin && x < edge$xmax)
      ys <- c(ys, edge$y)
  }
  ys <- sort(ys)
  if (length(ys) >= 2) matrix(ys, ncol = 2, byrow = TRUE) else matrix(numeric(0), ncol = 2)
})

getColumnIntervals <- function(x) {
  idx <- findInterval(x, colBreaks, rightmost.closed = FALSE)
  if (idx == 0 || idx > length(columnIntervals))
    return(matrix(numeric(0), ncol = 2))
  columnIntervals[[idx]]
}

segmentOnBoundaryHorizontal <- function(y, minX, maxX) {
  for (edge in HorizontalEdges) {
    if (edge$y == y &&
        minX >= edge$xmin &&
        maxX <= edge$xmax)
      return(TRUE)
  }
  FALSE
}

segmentOnBoundaryVertical <- function(x, minY, maxY) {
  for (edge in VerticalEdges) {
    if (edge$x == x &&
        minY >= edge$ymin &&
        maxY <= edge$ymax)
      return(TRUE)
  }
  FALSE
}


segmentWithinTiles <- function(a, b) {
  if (!(a[1] == b[1] || a[2] == b[2]))
    stop("Segment must be axis-aligned")

  if (a[1] == b[1]) {
    minY <- min(a[2], b[2])
    maxY <- max(a[2], b[2])

    if (segmentOnBoundaryVertical(a[1], minY, maxY))
      return(TRUE)

    intervals <- getColumnIntervals(a[1])
    any(intervals[, 1] <= minY & intervals[, 2] >= maxY)
  } else {
    minX <- min(a[1], b[1])
    maxX <- max(a[1], b[1])

    if (segmentOnBoundaryHorizontal(a[2], minX, maxX))
      return(TRUE)

    intervals <- getRowIntervals(a[2])
    any(intervals[, 1] <= minX & intervals[, 2] >= maxX)
  }
}

rectangleSegmentsWithinTiles <- function(p1, p2) {
  c1 <- c(x = p2["x"], y = p1["y"])
  c2 <- c(x = p1["x"], y = p2["y"])

  if (!pointInside(c1) || !pointInside(c2))
    return(FALSE)

  xs <- sort(c(p1["x"], p2["x"]))
  ys <- sort(c(p1["y"], p2["y"]))

  height <- ys[2] - ys[1] + 1
  width <- xs[2] - xs[1] + 1

  rowStartIdx <- match(ys[1], rowBreaks)
  rowEndIdx <- match(ys[2], rowBreaks)
  colStartIdx <- match(xs[1], colBreaks)
  colEndIdx <- match(xs[2], colBreaks)

  if (any(is.na(c(rowStartIdx, rowEndIdx, colStartIdx, colEndIdx))))
    stop("Rectangle corners must align with existing rows and columns")

  if (height <= width) {
    if (rowEndIdx > rowStartIdx) {
      for (idx in seq.int(rowStartIdx, rowEndIdx - 1)) {
        intervals <- rowIntervals[[idx]]
        if (NROW(intervals) == 0)
          return(FALSE)
        if (!any(intervals[, 1] <= xs[1] & intervals[, 2] >= xs[2]))
          return(FALSE)
      }
    }

    if (!segmentWithinTiles(
      c(x = xs[1], y = ys[2]),
      c(x = xs[2], y = ys[2])
    )) return(FALSE)
  } else {
    if (colEndIdx > colStartIdx) {
      for (idx in seq.int(colStartIdx, colEndIdx - 1)) {
        intervals <- columnIntervals[[idx]]
        if (NROW(intervals) == 0)
          return(FALSE)
        if (!any(intervals[, 1] <= ys[1] & intervals[, 2] >= ys[2]))
          return(FALSE)
      }
    }

    if (!segmentWithinTiles(
      c(x = xs[2], y = ys[1]),
      c(x = xs[2], y = ys[2])
    )) return(FALSE)
  }

  TRUE
}


areasu <- areas
areasu[upper.tri(areasu, diag = TRUE)] <- NA

combos <- expand.grid(idx, idx)
colnames(combos) <- c("i1", "i2")
combos$p1 <- I(coordinates[combos$i1])
combos$p2 <- I(coordinates[combos$i2])
combos$area <- as.vector(areasu)
combos <- combos[!is.na(combos$area), ]
combos <- combos[order(-combos$area), ]

bestArea <- -Inf
for (i in seq_len(NROW(combos))) {
  if (combos$area[i] <= bestArea)
    break

  if (rectangleSegmentsWithinTiles(combos$p1[[i]], combos$p2[[i]])) {
    bestArea <- combos$area[i]
    break
  }
}

cat("Answer Task 2: ", bestArea, "\n")
