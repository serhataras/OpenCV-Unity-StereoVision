using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour {

    public Animator anim;
    public Rigidbody rbody;

    private float m_MovementInputValue;
    private float m_TurnInputValue;

    public bool run;

    // Use this for initialization
    void Start () {
        anim = GetComponent<Animator>();
        rbody = GetComponent<Rigidbody>();
        run = false;
    }
	
	// Update is called once per frame
	void Update () {
        if (Input.GetKey("1"))
            anim.Play("WAIT01", -1, 0f);
        if (Input.GetKey("2"))
            anim.Play("WAIT02", -1, 0f);
        if (Input.GetKey("3"))
            anim.Play("WAIT03", -1, 0f);
        if (Input.GetKey("4"))
            anim.Play("WAIT04", -1, 0f);
        if (Input.GetKey("5"))
            anim.Play("WIN00", -1, 0f);
        if (Input.GetKey("6"))
            anim.Play("REFLESH00", -1, 0f);
        

        if (Input.GetMouseButtonDown(0))
            anim.Play("DAMAGED00", -1, 0);
        if (Input.GetMouseButtonDown(1))
            anim.Play("DAMAGED01", -1, 0);

        run = Input.GetKey(KeyCode.LeftShift);

        anim.SetBool("jump",    Input.GetKey(KeyCode.Space));
        anim.SetFloat("inputH", Input.GetAxis("Horizontal"));
        anim.SetFloat("inputV", Input.GetAxis("Vertical"));
        anim.SetBool("run",     run);
    }

    void FixedUpdate()
    {
        m_MovementInputValue = Input.GetAxis("Vertical");
        m_TurnInputValue = Input.GetAxis("Horizontal");
        Move();
        Turn();
    }

    private void Move()
    {
        // Create a vector in the direction the tank is facing with a magnitude based on the input, speed and the time between frames.
        Vector3 movement = transform.forward * m_MovementInputValue * Time.deltaTime;

        // Apply this movement to the rigidbody's position.
        if( run)
            rbody.MovePosition(rbody.position + 3f * movement);
        else
            rbody.MovePosition(rbody.position + movement);
    }


    private void Turn()
    {
        // Determine the number of degrees to be turned based on the input, speed and time between frames.
        float turn = m_TurnInputValue * 180 * Time.deltaTime;

        // Make this into a rotation in the y axis.
        Quaternion turnRotation = Quaternion.Euler(0f, turn, 0f);

        // Apply this rotation to the rigidbody's rotation.
        rbody.MoveRotation(rbody.rotation * turnRotation);
    }
}
