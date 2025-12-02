use std::num::ParseIntError;

use itertools::Itertools;

pub(crate) fn part_1(contents: &str) -> usize
{
    contents
        .trim()
        .split(',')
        .map(|string| {
            let mut split = string.split('-');
            Some((split.next()?, split.next()?))
        })
        .map(Option::unwrap)
        .map(|(first, second)| {
            Ok::<_, ParseIntError>((
                first.parse::<usize>()?,
                second.parse::<usize>()?
            ))
        })
        .map(|state| {
            let (first, second) = state.expect("Failed to parse range");
            (first ..= second)
                .filter(|num| {
                    let num = num.to_string();
                    num.len() & 1 == 0 &&
                        num[.. num.len() / 2] == num[num.len() / 2 ..]
                })
                .sum::<usize>()
        })
        .sum()
}

pub(crate) fn part_2(contents: &str) -> usize
{
    contents
        .trim()
        .split(',')
        .map(|string| {
            let mut split = string.split('-');
            Some((split.next()?, split.next()?))
        })
        .map(Option::unwrap)
        .map(|(first, second)| {
            Ok::<_, ParseIntError>((
                first.parse::<usize>()?,
                second.parse::<usize>()?
            ))
        })
        .map(|state| {
            let (first, second) = state.expect("Failed to parse range");
            (first ..= second)
                .filter(|num| {
                    let num = num.to_string();
                    let chunk_sizes: Vec<usize> = match num.len()
                    {
                        | 1 => vec![],
                        | 2 | 3 | 5 | 7 => vec![1],
                        | 4 => vec![1, 2],
                        | 6 => vec![1, 2, 3],
                        | 8 => vec![1, 2, 4],
                        | 9 => vec![1, 3],
                        | 10 => vec![1, 2, 5],
                        | 0 | 11 .. => unreachable!()
                    };

                    chunk_sizes
                        .iter()
                        .any(|&chunk_size| {
                            num.as_bytes()
                                .chunks(chunk_size)
                                .all_equal()
                        })
                })
                .sum::<usize>()
        })
        .sum()
}

#[cfg(test)]
mod test
{
    use super::*;
    const TEST: &str = "11-22,95-115,998-1012,1188511880-1188511890,\
                        222220-222224,1698522-1698528,446443-446449,\
                        38593856-38593862,565653-565659,824824821-824824827,\
                        2121212118-2121212124";
    const PART_1: usize = 1227775554;
    const PART_2: usize = 4174379265;

    #[test]
    fn part_1_test() { assert_eq!(part_1(TEST), PART_1) }
    #[test]
    fn part_2_test() { assert_eq!(part_2(TEST), PART_2) }
}
