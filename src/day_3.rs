use itertools::Itertools;
use rayon::{
    iter::ParallelIterator,
    str::ParallelString
};

pub(crate) fn part_1(contents: &str) -> usize
{
    contents
        .par_lines()
        .map(|line| {
            digits::best_from_digits(
                &digits::from_bytes(
                    line.trim()
                        .as_bytes()
                ),
                &2
            )
        })
        .sum()
}

pub(crate) fn part_2(contents: &str) -> usize
{
    contents
        .par_lines()
        .map(|line| {
            digits::best_from_digits(
                &digits::from_bytes(
                    line.trim()
                        .as_bytes()
                ),
                &12
            )
        })
        .sum()
}

mod digits
{
    use super::*;

    pub(super) fn from_bytes(string: &[u8]) -> Vec<u8>
    {
        string
            .iter()
            .map(|byte| byte - b'0')
            .collect_vec()
    }

    // fn best_from_digits_inner(
    //     digits: &[u8],
    //     len: &usize,
    //     curr_idx: usize,
    //     curr_digits: usize,
    //     curr_num: usize
    // ) -> usize
    // {

    //     if curr_digits == *len || curr_idx == digits.len()
    //     {
    //         return curr_num;
    //     }

    //     usize::max(
    //         best_from_digits_inner(
    //             digits,
    //             len,
    //             curr_idx + 1,
    //             curr_digits + 1,
    //             curr_num * 10 + digits[curr_idx] as usize
    //         ),
    //         best_from_digits_inner(
    //             digits,
    //             len,
    //             curr_idx + 1,
    //             curr_digits,
    //             curr_num
    //         )
    //     )
    // }

    pub(super) fn best_from_digits(
        digits: &[u8],
        len: &usize
    ) -> usize
    {
        // READ RECURSIVE FUNCTION IN COMMENTS ABOVE TO UNDERSTAND
        // THIS OPTIMIZATION

        // FIXME: current solution uses 2D array for DP, can be
        // improved
        let (rows, cols) = (len + 1, digits.len() + 1);
        let mut store = vec![vec![0_usize; cols]; rows];

        // FIXME: current solution uses reversed iteration for
        // filling, can be improved
        for row in (0 .. rows - 1).rev()
        {
            for col in (0 .. cols - 1).rev()
            {
                store[row][col] = usize::max(
                    store[row + 1][col + 1] +
                        10_usize.pow(row as u32) *
                            digits[digits.len() - col - 1] as usize,
                    store[row][col + 1]
                );
            }
        }

        store[0][0]
    }
}

#[cfg(test)]
mod test
{
    use super::*;

    const TEST: &str = "987654321111111
    811111111111119
    234234234234278
    818181911112111";

    const PART_1: usize = 357;
    const PART_2: usize = 3121910778619;

    #[test]
    fn part_1_test() { assert_eq!(part_1(TEST), PART_1) }
    #[test]
    fn part_2_test() { assert_eq!(part_2(TEST), PART_2) }
}
