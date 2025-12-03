use anyhow::Result;

mod day_1;
mod day_2;
mod day_3;

fn main() -> Result<()>
{
    let part1 = day_3::part_1(include_str!("../inputs/day_3.txt")); // src/../inputs/day_1.txt
    let part2 = day_3::part_2(include_str!("../inputs/day_3.txt")); // src/../inputs/day_1.txt
    println!("{part1}");
    println!("{part2}");
    Ok(())
}
