<?php

require_once(dirname(__FILE__).'/../php/clips.php');

class Dummy {
	public $hello;
	public $world;
	public function __construct() {
		$this->hello = 1;
		$this->world = '2';
	}

}

class ClipsTest extends PHPUnit_Framework_TestCase {
	public function setUp() {
		$this->clips = new Clips();
	}

	public function tearDown() {
	}

	public function testDefineFacts() {
		echo $this->clips->defineFacts('test_facts', array(new Dummy(),
			array('a', 'b', 'c')))."\n";
		echo $this->clips->assertFacts('test_facts', array(new Dummy(),
			array('a', 'b', 'c')))."\n";
	}

	public function testAddTemplate() {
		echo $this->clips->defineTemplate('Dummy')."\n";
	}

	public function testContextAcess() {
		$this->assertNotNull($this->clips);
		$this->clips->hello = "world";
	}

	public function testDefineSlot() {
		echo $this->clips->defineSlot('hello', 'slot', '1',
			array(
				array(
					'type' => 'type',
					'value' => 'INTEGER'
				),
				array(
					'type' => 'range',
					'begin' => 1,
					'end' => 10
				),
				array(
					'type' => 'allowed-integers',
					'value' => '1|2|3|4|5|6'
				)
		));
		echo "\n";
	}

	public function testDefineFact() {
		echo $this->clips->defineFact(array('a', 'b', 'c'))."\n";
		echo $this->clips->defineFact(new Dummy())."\n";
		echo $this->clips->defineFact(array('template'=>'test_template', 'a' => 1, 'b' => array(1, 2, 3)))."\n";
	}

	public function testDefineInstance() {
		echo $this->clips->defineInstance('hello')."\n";
	}

	public function testDefineClass() {
		echo $this->clips->defineClass('PHP_OBJECT', 
			array('USER', 'OBJECT'), false, array(
				'name', 'age', 
				array(
					'type' => 'multislot', 'name' => 'friends',
					'constraints' => array(
						array(
							'type' => 'range',
							'begin' => 1,
							'end' => 10
						),
						array(
							'type' => 'type',
							'value' => 'INTEGER'
						)
					)
				)
			)
		);
		echo "\n";
	}
}
