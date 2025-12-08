use anyhow::{
    Result,
    anyhow
};
use itertools::Itertools;

pub(crate) fn part_1(contents: &str) -> Result<usize>
{
    let lines = contents.lines();
    let mut problems = lines
        .map(|line| line.split_whitespace())
        .collect_vec();

    let operators = problems
        .pop()
        .ok_or(anyhow!("no lines??"))?
        .collect_vec();

    let mut result = 0;

    for operator in operators
    {
        let mut problem: usize = problems
            .first_mut()
            .and_then(|operator_iter| operator_iter.next())
            .and_then(|string| string.parse().ok())
            .ok_or(anyhow!("invalid grid"))?;
        for operand_iter in problems
            .iter_mut()
            .skip(1)
        {
            let operand: usize = operand_iter
                .next()
                .and_then(|string| string.parse().ok())
                .ok_or(anyhow!("invalid grid"))?;
            match operator
            {
                | "+" => problem += operand,
                | "*" => problem *= operand,
                | _ => unreachable!()
            }
        }
        result += problem;
    }

    Ok(result)
}

pub(crate) fn part_2(contents: &str) -> Result<usize>
{
    let mut lines: Vec<_> = contents
        .lines()
        .collect();

    let operators = lines
        .pop()
        .map(|str| str.chars())
        .ok_or(anyhow!("invalid input"))?
        .collect_vec();
    let operands = lines
        .into_iter()
        .map(|str| {
            str.bytes()
                .collect_vec()
        })
        .collect_vec();

    let mut result = 0;

    let max_len = operands
        .iter()
        .map(|str| str.len())
        .max()
        .unwrap_or_default();

    let mut indices = operators
        .iter()
        .enumerate()
        .filter(|(_, c)| !c.is_whitespace())
        .map(|(i, _)| i)
        .collect_vec();
    indices.push(max_len + 1);

    let mut buf = Vec::with_capacity(10);
    for i in 0 .. indices.len() - 1
    {
        (indices[i] .. indices[i + 1] - 1).for_each(|idx| {
            let mut count = 0_usize;
            for operands in &operands
            {
                if let Some(operand) = operands.get(idx) &&
                    operand.is_ascii_digit()
                {
                    count = count * 10 + (operand - b'0') as usize;
                }
            }
            buf.push(count);
        });

        result += match operators[indices[i]]
        {
            | '+' => buf
                .iter()
                .sum::<usize>(),
            | '*' => buf
                .iter()
                .product::<usize>(),
            | _ => unreachable!("unexpected {}", operators[i])
        };
        buf.clear();
    }

    Ok(result)
}

#[cfg(test)]
mod test
{
    use super::*;
    const TEST: &str = "\
123 328  51 64
 45 64  387 23
  6 98  215 314
*   +   *   +";
    const PART1: usize = 4277556;
    const PART2: usize = 3263827;

    #[test]
    fn part_1_test() { assert_eq!(part_1(TEST).unwrap(), PART1) }

    #[test]
    fn part_2_test() { assert_eq!(part_2(TEST).unwrap(), PART2) }
}
