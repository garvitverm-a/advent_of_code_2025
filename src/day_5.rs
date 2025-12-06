use std::cmp::Ordering;

use anyhow::{
    Result,
    anyhow
};
use rayon::{
    iter::{
        IntoParallelIterator,
        IntoParallelRefIterator,
        ParallelIterator
    },
    slice::ParallelSliceMut
};

#[derive(Debug, PartialEq, Eq)]
struct Range(usize, usize);

impl PartialOrd for Range
{
    fn partial_cmp(
        &self,
        other: &Self
    ) -> Option<Ordering>
    {
        Some(self.cmp(other))
    }
}

impl Ord for Range
{
    fn cmp(
        &self,
        other: &Self
    ) -> Ordering
    {
        self.0.cmp(&other.0)
    }
}

impl Range
{
    fn from_line(line: &str) -> Result<Self>
    {
        let (first, second) = line
            .trim()
            .split_once("-")
            .ok_or(anyhow!("invalid range"))?;
        Ok(Range(first.parse()?, second.parse()?))
    }

    fn cmp_with(
        &self,
        other: &usize
    ) -> Ordering
    {
        if self.0 > *other
        {
            Ordering::Greater
        }
        else if self.1 < *other
        {
            Ordering::Less
        }
        else
        {
            Ordering::Equal
        }
    }

    fn len(&self) -> usize { self.1 - self.0 + 1 }

    fn overlaps_with(
        &self,
        other: &Self
    ) -> bool
    {
        self.0 <= other.1 && other.0 <= self.1
    }

    fn merge_ranges(ranges: Vec<Self>) -> Vec<Self>
    {
        if ranges.is_empty()
        {
            return ranges;
        }
        let mut ranges = ranges;
        ranges.par_sort();
        let mut result = Vec::with_capacity(ranges.len());
        let mut ranges = ranges.into_iter();

        let mut curr = ranges
            .next()
            .unwrap();

        for next in ranges
        {
            if curr.overlaps_with(&next)
            {
                curr = curr.merge_with(&next);
            }
            else
            {
                result.push(curr);
                curr = next;
            }
        }
        result.push(curr);

        result
    }

    fn merge_with(
        &self,
        second: &Range
    ) -> Range
    {
        Range(self.0.min(second.0), self.1.max(second.1))
    }
}

pub(crate) fn part_1(contents: &str) -> Result<usize>
{
    let mut lines = contents.lines();

    let ranges: Result<Vec<_>> = lines
        .by_ref()
        .take_while(|line| !line.is_empty())
        .map(Range::from_line)
        .collect();

    let ranges = Range::merge_ranges(ranges?);

    let ingredients: Result<Vec<_>> = lines
        .map(|line| {
            Ok(line
                .trim()
                .parse::<usize>()?)
        })
        .collect();

    Ok(ingredients?
        .into_par_iter()
        .filter(|ingredient| {
            ranges
                .par_iter()
                .any(|range| {
                    range
                        .cmp_with(ingredient)
                        .is_eq()
                })
        })
        .count())
}

pub(crate) fn part_2(contents: &str) -> Result<usize>
{
    Ok(Range::merge_ranges(
        contents
            .lines()
            .take_while(|line| !line.is_empty())
            .map(Range::from_line)
            .collect::<Result<Vec<_>>>()?
    )
    .into_par_iter()
    .fold(|| 0_usize, |init, curr| init + curr.len())
    .sum())
}

#[cfg(test)]
mod test
{
    use super::*;

    const TEST: &str = "3-5
    10-14
    16-20
    12-18

    1
    5
    8
    11
    17
    32";

    #[test]
    fn part_1_test() { assert_eq!(part_1(TEST).unwrap(), 3) }
    #[test]
    fn part_2_test() { assert_eq!(part_2(TEST).unwrap(), 14) }
}
