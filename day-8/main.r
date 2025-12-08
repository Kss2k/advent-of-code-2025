file <- "input.txt"

input <- readLines(file) |>
  stringr::str_split_fixed(n = 3, pattern = ",") |>
  apply(MARGIN = 2, FUN = as.integer)

sinput <- readLines(file)

euclidean <- function(x, y) {
  sqrt(rowSums((x - y)^2))
}

euclidean_idx <- function(xi, yi) {
  euclidean(input[xi, ], input[yi, ])
}


idx <- seq_len(NROW(input))
dist <- outer(idx, idx, euclidean_idx)
dist[upper.tri(dist, diag = TRUE)] <- NA
pairs <- expand.grid(idx, idx)
pairs$distance <- as.vector(dist)
pairs$connected <- FALSE
pairs$Name1 <- sinput[pairs$Var1]
pairs$Name2 <- sinput[pairs$Var2]

connectPairs <- function(pairs, n = 1000, connectAll = FALSE) {
  pairs <- pairs[order(pairs$distance), ]

  clusters <- data.frame(idx = idx, coordinate = sinput, cluster = NA)
  k <- 1
  c <- 1

  sameCluster <- function(cx, cy) {
    !is.na(cx) && !is.na(cy) && cx == cy
  }

  for (i in seq_len(NROW(pairs))) {
    if ((k > n && !connectAll) || 
        (connectAll && (!any(is.na(clusters$cluster)) && length(unique(clusters$cluster)) == 1))) break
    k <- k + 1

    x <- pairs$Var1[i]
    y <- pairs$Var2[i]
    nx <- pairs$Name1[i]
    ny <- pairs$Name2[i]
    cx <- clusters$cluster[x]
    cy <- clusters$cluster[y]

    if (sameCluster(cx, cy)) {
      cat(sprintf("Not connecting %s %s\n", nx, ny))
      next
    }

    if (is.na(cx) && is.na(cy)) {
      cx <- cy <- c
      clusters$cluster[c(x, y)] <- cx
      c <- c + 1

    } else if (is.na(cx)) {
      cx <- cy
      clusters$cluster[c(x, y)] <- cx

    } else if (is.na(cy)) {
      cy <- cx
      clusters$cluster[c(x, y)] <- cy

    } else {
      # this is fucked
      clusters$cluster[clusters$cluster == cy] <- cx
    }
  }

  if (!connectAll) {
    offset <- max(clusters$cluster, na.rm = TRUE)
    clusters$cluster[is.na(clusters$cluster)] <- seq(offset + 1, offset + sum(is.na(clusters$cluster)))

    freq <- sort(table(clusters$cluster), decreasing = TRUE)
    prod(freq[1:3])
  } else {
    xcord_x <- input[x, 1]
    xcord_y <- input[y, 1]
    gmp::as.bigz(xcord_x) * gmp::as.bigz(xcord_y)
  }
}

connectPairs(pairs, 1000)
connectPairs(pairs, connectAll = TRUE)
