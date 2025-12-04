input <- readLines("input.txt") |>
  stringr::str_split(",") |>
  unlist() |>
  stringr::str_split_fixed("-", 2)

getRange <- function(x) {
  x <- as.integer(x)
  as.character(seq(x[1L], x[2L]))
}


getCharacterRange <- function(x) {
  x0 <- x[1L]
  x1 <- x[2L]

  w <- max(nchar(x))

  x0 <- stringr::str_pad(x0, side = "left", width = w, pad = "0")
  x1 <- stringr::str_pad(x1, side = "left", width = w, pad = "0")

  for (i in seq_len(w)) {
    c0 <- as.integer(substr(x0, i, i))
    c1 <- as.integer(substr(x1, i, i))
    if (c0 != c1) break
  }

  r0 <- as.integer(substr(x0, start = i, stop = w))
  r1 <- as.integer(substr(x1, start = i, stop = w))
  range <- stringr::str_pad(as.character(seq(r0, r1)), side = "left", width = w - i + 1, pad = "0")
  
  if (i > 1)
    range <- paste0(substr(x0, start = 1, stop = i-1), range)

  stringr::str_remove(range, pattern = "^0+")
}

getRanges <- function(strings) {
  apply(strings, MARGIN = 1L, FUN = getCharacterRange)
}


isRepeating2FixedWidth <- function(x) {
  width <- unique(nchar(x))
  stopifnot(length(width) == 1L)

  k         <- floor(width/2L)
  npatterns <- 2

  repeating <- logical(length(x))
  if (width <= 1L || width %% k != 0)
    return(repeating)

  pattern   <- substr(x, start = 1L, stop = k)
  strrep(pattern, npatterns) == x
}


isRepeating2 <- function(x) {
  widths <- nchar(x)

  repeating <- logical(length(x))
  for (width in unique(widths)) {
    cond <- widths == width
    repeating[cond] <- isRepeating2FixedWidth(x[cond])
  }

  repeating
}


isRepeatingFixedWidth <- function(x) {
  width <- unique(nchar(x))
  stopifnot(length(width) == 1L)

  K <- floor(width/2L)

  repeating <- logical(length(x))
  for (k in seq_len(K)) {
    if (width %% k != 0)
      next

    pattern   <- substr(x, start = 1L, stop = k)
    npatterns <- width / k
    stopifnot(floor(npatterns) == npatterns)

    repeating <- repeating | strrep(pattern, npatterns) == x
  }

  repeating
}


isRepeating <- function(x) {
  widths <- nchar(x)

  repeating <- logical(length(x))
  for (width in unique(widths)) {
    cond <- widths == width
    repeating[cond] <- isRepeatingFixedWidth(x[cond])
  }

  repeating
}

ids <- unlist(getRanges(input))
repeating2 <- isRepeating2(ids)
sum(gmp::as.bigz(ids[repeating2]), na.rm = TRUE)

repeating <- isRepeating(ids)
sum(gmp::as.bigz(ids[repeating]), na.rm = TRUE)
