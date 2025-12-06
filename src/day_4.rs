use std::ops::Add;

use itertools::Itertools;

const DIRECTIONS: [(i8, i8); 8] = [
    (-1, -1),
    (-1, 0),
    (-1, 1),
    (0, -1),
    (0, 1),
    (1, -1),
    (1, 0),
    (1, 1)
];

#[derive(Debug, Clone, Copy)]
struct Point(usize, usize);

impl Add<(i8, i8)> for Point
{
    type Output = Option<Self>;

    fn add(
        self,
        rhs: (i8, i8)
    ) -> Self::Output
    {
        Some(Point(
            self.0
                .checked_add_signed(rhs.0 as isize)?,
            self.1
                .checked_add_signed(rhs.1 as isize)?
        ))
    }
}

impl Add for Point
{
    type Output = Option<Self>;

    fn add(
        self,
        rhs: Self
    ) -> Self::Output
    {
        Some(Point(
            self.0
                .checked_add(rhs.0)?,
            self.1
                .checked_add(rhs.1)?
        ))
    }
}

pub(crate) fn part_1(contents: &str) -> usize
{
    let lines = contents
        .lines()
        .collect_vec();

    let len = lines.len();
    let inner_len = lines
        .first()
        .map(|&s| s.len())
        .unwrap_or_default();

    let grid = lines
        .into_iter()
        .flat_map(|s| {
            s.trim()
                .bytes()
                .map(|c| c == b'@')
        })
        .collect_vec();

    let idx = |Point(x, y)| x * inner_len + y;
    let mut count = 0;

    for i in 0 .. len
    {
        for j in 0 .. inner_len
        {
            let curr = Point(i, j);
            count += (grid[idx(curr)] &&
                DIRECTIONS
                    .iter()
                    .map(|&ds| curr + ds)
                    .filter(|point| {
                        point.is_some_and(|point| {
                            (point.0 < len && point.1 < inner_len) &&
                                grid[idx(point)]
                        })
                    })
                    .count() <
                    4) as usize;
        }
    }

    count
}

pub(crate) fn part_2(contents: &str) -> usize
{
    let lines = contents
        .lines()
        .collect_vec();

    let len = lines.len();
    let inner_len = lines
        .first()
        .map(|&s| s.len())
        .unwrap_or_default();

    let mut grid = lines
        .into_iter()
        .flat_map(|s| {
            s.trim()
                .bytes()
                .map(|c| c == b'@')
        })
        .collect_vec();

    let idx = |Point(x, y)| x * inner_len + y;
    let mut count = 0;

    let mut progress = true;
    while progress
    {
        progress = false;
        for i in 0 .. len
        {
            for j in 0 .. inner_len
            {
                let curr = Point(i, j);

                if grid[idx(curr)] &&
                    DIRECTIONS
                        .iter()
                        .map(|&ds| curr + ds)
                        .filter(|point| {
                            point.is_some_and(|point| {
                                (point.0 < len && point.1 < inner_len) &&
                                    grid[idx(point)]
                            })
                        })
                        .count() <
                        4
                {
                    count += 1;
                    grid[idx(curr)] = false;
                    progress = true;
                }
            }
        }
    }

    count
}

#[cfg(test)]
mod test
{
    use super::*;

    const TEST: &str = "..@@.@@@@.
    @@@.@.@.@@
    @@@@@.@.@@
    @.@@@@..@.
    @@.@@@@.@@
    .@@@@@@@.@
    .@.@.@.@@@
    @.@@@.@@@@
    .@@@@@@@@.
    @.@.@@@.@.";

    #[test]
    fn part_1_test() { assert_eq!(part_1(TEST), 13) }
    #[test]
    fn part_2_test() { assert_eq!(part_2(TEST), 43) }
}
