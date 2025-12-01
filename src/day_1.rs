use std::num::ParseIntError;

use anyhow::Result;

const DIAL_START: i16 = 50;
const DIAL_MAX: i16 = 100;

pub(crate) fn part_1(contents: &str) -> Result<usize>
{
    Ok(contents
        .lines()
        .map(str::trim)
        .filter(|string| !string.is_empty())
        .fold(Ok::<_, ParseIntError>((DIAL_START, 0_usize)), |state, line| {
            let (dial, password) = state?;
            let (direction, rotation) = line.split_at(1);

            let rotation: i16 = rotation.parse::<i16>()? *
                match direction
                {
                    | "L" => -1,
                    | "R" => 1,
                    | _ => 0
                };
            let dial = (dial + rotation).rem_euclid(DIAL_MAX);
            Ok((dial, password + (dial == 0) as usize))
        })?
        .1)
}

pub(crate) fn part_2(contents: &str) -> Result<usize>
{
    Ok(contents
        .lines()
        .map(str::trim)
        .filter(|string| !string.is_empty())
        .fold(Ok::<_, ParseIntError>((DIAL_START, 0_usize)), |state, line| {
            let (dial, password) = state?;

            let (direction, rotation) = line.split_at(1);
            let rotation: i16 = rotation.parse::<i16>()? *
                match direction
                {
                    | "L" => -1,
                    | "R" => 1,
                    | _ => 0
                };

            let sum = dial + rotation;

            let password = password +
                (sum / DIAL_MAX).abs() as usize +
                (dial != 0 && sum <= 0) as usize;
            let dial = sum.rem_euclid(DIAL_MAX);

            Ok((dial, password))
        })?
        .1)
}

#[cfg(test)]
mod test
{
    use super::*;
    const TEST: &'static str = "L68
    L30
    R48
    L5
    R60
    L55
    L1
    L99
    R14
    L82";

    #[test]
    fn part_1_test() { assert_eq!(part_1(TEST).unwrap(), 3) }
    #[test]
    fn part_2_test() { assert_eq!(part_2(TEST).unwrap(), 6) }
}
