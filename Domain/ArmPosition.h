#ifndef ARMPOSITION_H
#define ARMPOSITION_H



class ArmPosition {
private:
    float xPosition_;
    float yPosition_;
    float zPosition_;
    float xDegrees_;
    float yDegrees_;
    float zDegrees_;
    float gripForce_;

public:
    ArmPosition(float xPosition, float yPosition, float zPosition, float xDegrees, float yDegrees, float zDegrees, float gripForce);

    ~ArmPosition();

    [[nodiscard]] float get_x_position() const;

    void set_x_position(float x_position);

    [[nodiscard]] float get_y_position() const;

    void set_y_position(float y_position);

    [[nodiscard]] float get_z_position() const;

    void set_z_position(float z_position);

    [[nodiscard]] float get_x_degrees() const;

    void set_x_degrees(float x_degrees);

    [[nodiscard]] float get_y_degrees() const;

    void set_y_degrees(float y_degrees);

    [[nodiscard]] float get_z_degrees() const;

    void set_z_degrees(float z_degrees);

    [[nodiscard]] float get_grip_force() const;

    void set_grip_force(float grip_force);
};



#endif //ARMPOSITION_H
